#include <QtCore>
#include <QtGui>
#include <QtWebKit>
#include <zlib.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "torrent_hash_convert.h"
#include "Archive.h"
#include "ArchiveImpl.h"
#include "quazip/quagzipfile.h"

enum {
    COLUMN_ID,
    COLUMN_NAME,
    COLUMN_SIZE,
    COLUMN_SEEDS,
    COLUMN_LEECHES,
    COLUMN_HASH,
    COLUMN_DOWNLOADS,
    COLUMN_UPDATED
};

QByteArray gUncompress(const QByteArray &data) {
    if(data.size()<=4){
        qWarning("gUncompress: Input data is truncated");
        return QByteArray();
    }
    QByteArray result;
    int ret;
    z_stream strm;
    static const int CHUNK_SIZE = 1024;
    char out[CHUNK_SIZE];
    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = data.size();
    strm.next_in = (Bytef*)(data.data());
    ret = inflateInit2(&strm,15+32); // gzip decoding
    if (ret != Z_OK) {
        return QByteArray();
    }
    // run inflate()
    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)(out);

        ret = inflate(&strm, Z_NO_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR);  // state not clobbered
        switch (ret){
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     // and fall through
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return QByteArray();
        }
        result.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);
    // clean up and return
    inflateEnd(&strm);
    return result;
}

bool gUncompress(QFile* in,QFile* out) {
    if(!in->open(QIODevice::ReadOnly) || !out->open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        return false;
    }
    gzFile gz = gzdopen(in->handle(), "rb");
    if (gz == NULL) {
        return false;
    }
    static const int buffer_size = 0x40000;
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);
    int read = 0;
    do {
        out->write(QByteArray(buffer, read*sizeof(char)));
        read = qMin(gzread(gz, buffer, buffer_size), buffer_size);
    } while(read > 0);
    gzclose(gz);
    return true;
}

MainWindow* mainWindow() {
    return static_cast<MainWindow*>(QApplication::activeWindow());
}

QString rutrackerUrl(int id) {
    return "http://rutracker.org/forum/viewtopic.php?t=" + QString::number(id);
}

QString magnetUrl(QString hash, QString name, qlonglong size, bool urlencode) {
    return "magnet:?xt=urn:btih:" + hash +
        "&dn=" + (urlencode ? QUrl::toPercentEncoding(name) : name) +
        "&xl=" + QString::number(size);
}


class IDItem : public QTableWidgetItem {
public:
    IDItem(int id):
        id_(id) {
        setData(Qt::DisplayRole, id);
        setData(Qt::ForegroundRole, Qt::blue);
        setFlags(flags() & ~Qt::ItemIsSelectable);
    }

    bool operator<(const QTableWidgetItem& other) const {
        const IDItem* other_sized = dynamic_cast<const IDItem*>(&other);
        if (other_sized) {
            return id_ < other_sized->id_;
        } else {
            return this->QTableWidgetItem::operator<(other);
        }
    }

    qlonglong id() const {
        return id_;
    }
    friend class IDDelegate;
    friend class MainWindow;
private:
    qlonglong id_;
};

class IDDelegate : public QItemDelegate {
public:
    IDDelegate(QTableWidget* table, QObject* parent = 0):
        QItemDelegate(parent), table_(table) {
    }

    bool editorEvent(QEvent* event, QAbstractItemModel*,
                     const QStyleOptionViewItem&, const QModelIndex& index) {
        qDebug() << "IDDelegate.editorEvent";
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);
            if (e) {
                int row = index.row();
                int col = COLUMN_ID;
                IDItem* item = dynamic_cast<IDItem*>(table_->item(row, col));
                if (item) {
                    int id = item->id_;
                    QString url = rutrackerUrl(id);
                    if (e->button() == Qt::LeftButton) {
                        mainWindow()->openUrl(url);
                    } else if (e->button() == Qt::RightButton) {
                        QApplication::clipboard()->setText(url);
                    }
                    return true;
                }
            }
        }
        return false;
    }

private:
    QTableWidget* table_;
};

class HashItem : public QTableWidgetItem {
public:
    HashItem(QString hash, QString name, qlonglong size):
        base32_hash_(torrent_hash_convert(hash, /*base32*/ true)),
        name_(name), size_(size) {
        updateHash();
        setData(Qt::ForegroundRole, Qt::blue);
        setFlags(flags() & ~Qt::ItemIsSelectable);
    }

    void updateHash() {
        QString h = hash();
        setData(Qt::DisplayRole, h);
    }

    QString hash() {
        MainWindow* w = mainWindow();
        if (w && w->useBase32()) {
            return base32_hash_; // base32
        } else {
            return torrent_hash_convert(base32_hash_, /*base32*/ false); // hex
        }
    }

    friend class HashDelegate;
    friend class MainWindow;

private:
    QString base32_hash_; // base32
    QString name_;
    qlonglong size_;
};

class HashDelegate : public QItemDelegate {
public:
    HashDelegate(QTableWidget* table, QObject* parent = 0):
        QItemDelegate(parent), table_(table) {
    }

    bool editorEvent(QEvent* event, QAbstractItemModel*,
                     const QStyleOptionViewItem&, const QModelIndex& index) {
        qDebug() << "HashDelegate.editorEvent";
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);
            if (e) {
                int row = index.row();
                int col = COLUMN_HASH;
                HashItem* item = dynamic_cast<HashItem*>(table_->item(row, col));
                if (item) {
                    if (e->button() == Qt::LeftButton) {
                        QString url = magnetUrl(item->hash(), item->name_, item->size_, false);
                        QDesktopServices::openUrl(url);
                    } else if (e->button() == Qt::RightButton) {
                        QString url = magnetUrl(item->hash(), item->name_, item->size_, true);
                        QApplication::clipboard()->setText(url);
                    }
                    return true;
                }
            }
        }
        return false;
    }

private:
    QTableWidget* table_;
};

SearchingThread::SearchingThread(QIODevice* input, MainWindow* window,
                                 int limit, QString pattern, bool cp1251):
    input_(input), window_(window),
    limit_(limit), pattern_(pattern), cp1251_(cp1251) {
}

void SearchingThread::run() {
    QTextStream in(input_);
    if (cp1251_) {
        in.setCodec(QTextCodec::codecForName("Windows-1251"));
    } else {
        in.setCodec(QTextCodec::codecForName("UTF-8"));
    }
    if (limit_ <= 0) {
        emit stopFilling();
        return;
    }
    int hits = 0;
    QStringList_ptr chunk(new QStringList);
    QByteArray line_byte;
    while(0!=(line_byte=input_->readLine())) {
        if (!window_->keepSearching()) {
            emit newLines(chunk);
            chunk = QStringList_ptr(new QStringList);
            emit stopFilling();
            return;
        }
        QString line=QString::fromUtf8(line_byte);
        QStringList fields = line.split('\t');
        if (fields.size() != 8) {
            fields = line.split('|');
        }
        if (fields.size() < 8) {
            continue;
        }
        QString name = fields[1];
        if (name.contains(pattern_, Qt::CaseInsensitive)) {
            (*chunk) << line;
            hits += 1;
            if (hits >= limit_) {
                emit newLines(chunk);
                chunk = QStringList_ptr(new QStringList);
                emit stopFilling();
                return;
            }
            if (chunk->size() >= 5) {
                emit newLines(chunk);
                chunk = QStringList_ptr(new QStringList);
            }
        }
    }
    emit newLines(chunk);
    emit stopFilling();
}

QDir appDir() {
    return QFileInfo(QCoreApplication::applicationFilePath()).absoluteDir();
}

QString settingsPath() {
    return appDir().absoluteFilePath("dump_viewer.ini");
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    keepSearching_(false),
    settings_(settingsPath(), QSettings::IniFormat),
    useBase32_(false)
{
    qRegisterMetaType<QStringList_ptr>("QStringList_ptr");
    ui->setupUi(this);
    QTableWidget* table = ui->resultsTableWidget;
    table->setItemDelegateForColumn(COLUMN_ID, new IDDelegate(table, this));
    table->setItemDelegateForColumn(COLUMN_HASH, new HashDelegate(table, this));
    table->resizeColumnsToContents();
    table->setSortingEnabled(true);
    table->sortByColumn(COLUMN_DOWNLOADS, Qt::DescendingOrder);
    if (settings().contains("pattern")) {
        ui->patternLineEdit->setText(settings().value("pattern").toString());
    }
    if (settings().contains("limit")) {
        ui->limitSpinBox->setValue(settings().value("limit").toInt());
    }
    if (settings().contains("use_base32")) {
        useBase32_ = settings().value("use_base32").toBool();
    }
    ui->base32Action->setChecked(useBase32());
    table->resizeColumnsToContents();
    table->setColumnWidth(COLUMN_ID, table->columnWidth(COLUMN_ID) + 10);
    table->setColumnWidth(COLUMN_HASH, 150);
    table->horizontalHeader()->setResizeMode(COLUMN_NAME, QHeaderView::Stretch);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    updateButtons();
    connect(table->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(rowChanged(QModelIndex)));
    table->addAction(ui->action_copy_rutracker_link);
    table->addAction(ui->action_open_rutracker);
    table->addAction(ui->action_copy_magnet);
    table->addAction(ui->action_open_magnet);
    QWebSettings* ws = ui->descriptionWebView->settings();
    ws->setAttribute(QWebSettings::JavascriptEnabled, false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startFilling() {
    QTableWidget* table = ui->resultsTableWidget;
    table->clearContents();
    table->setRowCount(0);
    table->setSortingEnabled(false);
    keepSearching_ = true;
    updateButtons();
}

void MainWindow::stopFilling() {
    QTableWidget* table = ui->resultsTableWidget;
    table->setSortingEnabled(true);
    keepSearching_ = false;
    updateButtons();
}

void MainWindow::showDescription(int id) {
    qDebug() << "showDescription " << id;
    QString description = descriptionById(id);
    ui->descriptionWebView->setHtml(description);
}

void MainWindow::rowChanged(QModelIndex current) {
    QTableWidget* table = ui->resultsTableWidget;
    int row = current.row();
    int column = COLUMN_ID;
    QTableWidgetItem* item = table->item(row, column);
    IDItem* id_item = dynamic_cast<IDItem*>(item);
    if (id_item) {
        showDescription(id_item->id());
    }
}

QString fileSize(qlonglong size) {
    if (size > 1024 * 1024 * 1024) {
        size /= 1024 * 1024 * 1024;
        return QString::number(size) + " GB";
    }
    if (size > 1024 * 1024) {
        size /= 1024 * 1024;
        return QString::number(size) + " MB";
    }
    if (size > 1024) {
        size /= 1024;
        return QString::number(size) + " kB";
    }
    return QString::number(size) + " B";
}

QDateTime parseUpdated(QString text) {
    text.replace("Jan", "01");
    text.replace("Feb", "02");
    text.replace("Mar", "03");
    text.replace("Apr", "04");
    text.replace("May", "05");
    text.replace("Jun", "06");
    text.replace("Jul", "07");
    text.replace("Aug", "08");
    text.replace("Sep", "09");
    text.replace("Oct", "10");
    text.replace("Nov", "11");
    text.replace("Dec", "12");
    QDateTime datetime = QDateTime::fromString(text, "dd-MM-yy HH:mm");
    if (datetime.date().year() < 2000) {
        // shit like 1909 on place of 2009
        datetime = datetime.addYears(100);
    }
    return datetime;
}

class SizeItem : public QTableWidgetItem {
public:
    SizeItem(qlonglong size):
        size_(size) {
        setData(Qt::ToolTipRole, size);
        setData(Qt::DisplayRole, fileSize(size));
    }

    bool operator<(const QTableWidgetItem& other) const {
        const SizeItem* other_sized = dynamic_cast<const SizeItem*>(&other);
        if (other_sized) {
            return size_ < other_sized->size_;
        } else {
            return this->QTableWidgetItem::operator<(other);
        }
    }

private:
    qlonglong size_;
};

void MainWindow::on_searchButton_clicked() {
    search();
}

void MainWindow::on_stopButton_clicked() {
    stopFilling();
}

void MainWindow::on_exitAction_triggered() {
    qApp->quit();
}

void MainWindow::on_selectAction_triggered() {
    QString path = appDir().absolutePath();
    if (settings().contains("final_txt")) {
        path = settings().value("final_txt").toString();
    }
    path = QFileDialog::getOpenFileName(this,
            tr("Provide path to database"), path);
    settings().setValue("final_txt", path);
    qDebug() << path;
}

void MainWindow::on_base32Action_triggered(bool base32) {
    useBase32_ = base32;
    settings().setValue("use_base32", base32);
    QTableWidget* table = ui->resultsTableWidget;
    for (int row = 0; row < table->rowCount(); row++) {
        QTableWidgetItem* item = table->item(row, COLUMN_HASH);
        HashItem* hash_item = dynamic_cast<HashItem*>(item);
        if (hash_item) {
            hash_item->updateHash();
        }
    }
}

void MainWindow::on_cp1251Action_triggered(bool cp1251) {
    settings().setValue("cp1251", cp1251);
}

void MainWindow::on_selectDescriptionAction_triggered() {
    QString path = appDir().absolutePath();
    if (settings().contains("descriptions_root")) {
        path = settings().value("descriptions_root").toString();
    }
    path = QFileDialog::getExistingDirectory(this,
            tr("Provide path to descriptions database"), path);
    settings().setValue("descriptions_root", path);
    qDebug() << path;
}

void MainWindow::openUrl(QString url) {
    ui->descriptionWebView->load(url);
    ui->descriptionWebView->show();
}

void MainWindow::on_patternLineEdit_returnPressed() {
    if (!keepSearching()) {
        search();
    } else {
        stopFilling();
        QTimer::singleShot(500, this, SLOT(search()));
    }
}

QString backupName(QString path) {
    for (int i = 0;; i++) {
        QString name = path + "." + QString::number(i);
        if (!QFile(name).exists()) {
            return name;
        }
    }
}

void MainWindow::on_unpackAction_triggered() {
    QString zip_path = settings().value("final_txt").toString();
    if (!QFile(zip_path).exists()) {
        QErrorMessage::qtHandler()->showMessage(tr("Select database existing file first"));
        return;
    }
    if (!zip_path.endsWith(".gz")) {
        QErrorMessage::qtHandler()->showMessage(tr("Select compressed database file first"));
        return;
    }
    qDebug() << "Unpack " << zip_path;
    QFile inputFile(zip_path);
    if (inputFile.open(QIODevice::ReadOnly)) {
        QStringList zip_path_split = zip_path.split('.');
        zip_path_split.pop_back(); // remove .gz etc
        QString path = zip_path_split.join(".");
        if (QFile(path).exists()) {
            QFile(path).rename(backupName(path));
        }
        QFile outputFile(path);
        inputFile.close();
        if (gUncompress(&inputFile,&outputFile)) {
            settings().setValue("final_txt", path);
        } else {
            QErrorMessage::qtHandler()->showMessage(tr("Error openning output database file!"));
        }
    } else {
        QErrorMessage::qtHandler()->showMessage(tr("Error openning input database file!"));
    }
}

void MainWindow::addLines(QStringList_ptr lines) {
    if (!keepSearching()) {
        return;
    }
    QTableWidget* table = ui->resultsTableWidget;
    foreach (const QString& line, *lines) {
        QStringList fields = line.split('\t');
        if (fields.size() != 8) {
            fields = line.split('|');
        }
        int id = fields[0].toInt();
        QString name = fields[1];
        qlonglong size = fields[2].toLongLong();
        int seeds = fields[3].toInt();
        int leeches = fields[4].toInt();
        QString hash = fields[5];
        qlonglong downloads = fields[6].toLongLong();
        QDateTime updated = parseUpdated(fields[7]);
        table->insertRow(table->rowCount());
        int row = table->rowCount() - 1;
        table->setItem(row, COLUMN_ID, new IDItem(id));
        setData(row, COLUMN_NAME, name);
        table->setItem(row, COLUMN_SIZE, new SizeItem(size));
        setData(row, COLUMN_SEEDS, seeds);
        setData(row, COLUMN_LEECHES, leeches);
        table->setItem(row, COLUMN_HASH, new HashItem(hash, name, size));
        setData(row, COLUMN_DOWNLOADS, downloads);
        setData(row, COLUMN_UPDATED, updated);
    }
}

QIODevice* MainWindow::getInputDevice() {
    QString input_path;
    if (input_path.isEmpty() && settings().contains("final_txt")) {
        QString path = settings().value("final_txt").toString();
        if (QFile(path).exists()) {
            input_path = path;
        }
    }
    QDir dir = appDir();
    if (input_path.isEmpty() && QFile(dir.absoluteFilePath("final.txt")).exists()) {
        input_path = dir.absoluteFilePath("final.txt");
    }
    if (input_path.isEmpty() && QFile(dir.absoluteFilePath("final.txt.gz")).exists()) {
        input_path = dir.absoluteFilePath("final.txt.gz");
    }
    if (input_path.isEmpty()) {
        on_selectAction_triggered();
        if (settings().contains("final_txt")) {
            QString path = settings().value("final_txt").toString();
            if (QFile(path).exists()) {
                input_path = path;
            }
        }
    }
    //InputPtr input;
    if (!input_path.isEmpty() && QFile(input_path).exists()) {
        qDebug() << input_path;
        if (input_path.endsWith(".txt")) {
            return new QFile(input_path);
        } else if (input_path.endsWith(".gz")) {
            qDebug()<<"eh...";
            return new QuaGzipFile(input_path);
            //gz->open(QIODevice::ReadOnly);
            //qDebug()<<gz->read(500);
            //input = InputPtr(gz);
        }
    }
    return new QFile();
}

void MainWindow::search() {
    QString pattern = ui->patternLineEdit->text();
    int limit = ui->limitSpinBox->value();
    settings().setValue("pattern", pattern);
    settings().setValue("limit", limit);
    QIODevice* input = getInputDevice();
    if (input && input->open(QIODevice::ReadOnly)) {
        startFilling();
        bool cp1251 = false;
        if (settings().contains("cp1251") && settings().value("cp1251").toBool()) {
            cp1251 = true;
        }
        SearchingThread* thread = new SearchingThread(input, this, limit, pattern, cp1251);
        connect(thread, SIGNAL(newLines(QStringList_ptr)),
                this, SLOT(addLines(QStringList_ptr)),
                Qt::QueuedConnection);
        connect(thread, SIGNAL(stopFilling()),
                this, SLOT(stopFilling()),
                Qt::QueuedConnection);
        QThreadPool::globalInstance()->start(thread);
    } else {
        QErrorMessage::qtHandler()->showMessage(tr("Error openning database file!"));
    }
}

void MainWindow::updateButtons() {
    if (keepSearching()) {
        ui->searchButton->hide();
        ui->stopButton->show();
    } else {
        ui->searchButton->show();
        ui->stopButton->hide();
    }
}

QString MainWindow::descriptionById(int id) {
    QDir dir0 = appDir();
    if (settings().contains("descriptions_root")) {
        dir0 = settings().value("descriptions_root").toString();
    }
    QString id_c = QString("%1").arg(id, /*width*/ int(8), /*base*/ int(10), QChar('0'));
    QString dir = id_c.left(3);
    dir0.cd(dir);
    QString tar = id_c.left(5) + ".tar.gz";
    QString tar_abs = dir0.absoluteFilePath(tar);
    if (!QFile(tar_abs).exists()) {
        tar_abs += ".gz";
        if (!QFile(tar_abs).exists()) {
            qDebug() << "!QFile(tar_abs).exists()" << tar_abs;
            return "";
        }
    }
    qDebug() << tar_abs;
    QFile tar_gz_file(tar_abs);
    if(!tar_gz_file.open(QIODevice::ReadOnly)){
        qDebug()<<"!tar_gz_file.open"; return "";
    }
    QByteArray tar_gz_ba=tar_gz_file.readAll(); // _ba means QByteArray, _file means QFile
    tar_gz_file.close();
    QByteArray tar_ba=gUncompress(tar_gz_ba);
    QBuffer tar_buf; tar_buf.open(QIODevice::ReadWrite); tar_buf.write(tar_ba); tar_buf.close();
    bugless::Archive tar_ar(&tar_buf,(bugless::Archive::Type)2);
    if(!tar_ar.valid()){
        qDebug()<<"tar archive turned invalid!"; return "";
    }
    QIODevice* desc_dev;
    if((desc_dev=tar_ar.device(id_c))==NULL){
        qDebug()<<"somehow failed to extract description file from tar archive"; return "";
    }
    desc_dev->open(QIODevice::ReadOnly);
    QString description=QString::fromUtf8(desc_dev->readAll());
    desc_dev->close();
    return description;
}

IDItem* MainWindow::get_current_item()
{
    QTableWidget* table = ui->resultsTableWidget;
    QModelIndex index = table->currentIndex();
    int row = index.row();
    int col = COLUMN_ID;
    if (row >= 0)
    {
        return dynamic_cast<IDItem*>(table->item(row, col));
    }
    else
        return NULL;
//    return item;
}

HashItem* MainWindow::get_current_hash_item()
{
    QTableWidget* table = ui->resultsTableWidget;
    QModelIndex index = table->currentIndex();
    int row = index.row();
    qDebug() << row;
    if (row >= 0)
    {
        int col = COLUMN_HASH;
        return dynamic_cast<HashItem*>(table->item(row, col));
    }
    return NULL;

//    return item;
}

void MainWindow::setData(int row, int col, const QVariant& data) {
    QTableWidgetItem* item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, data);
    QTableWidget* table = ui->resultsTableWidget;
    table->setItem(row, col, item);
}

void MainWindow::on_action_copy_rutracker_link_triggered()
{
    IDItem* item = get_current_item();
    if (item)
    {
        QString url = rutrackerUrl(item->id());
        QApplication::clipboard()->setText(url);
    }
}

void MainWindow::on_action_open_rutracker_triggered()
{
    IDItem* item = get_current_item();
    if (item)
    {
        QString url = rutrackerUrl(item->id());
        mainWindow()->openUrl(url);
    }
}

void MainWindow::on_action_copy_magnet_triggered()
{
    HashItem* item = get_current_hash_item();
    if (item)
    {
        QString url = magnetUrl(item->hash(), item->name_, item->size_, true);
        QApplication::clipboard()->setText(url);
    }
}

void MainWindow::on_action_open_magnet_triggered()
{
    HashItem* item = get_current_hash_item();
    if (item)
    {
        QString url = magnetUrl(item->hash(), item->name_, item->size_, false);
        QDesktopServices::openUrl(url);
    }
}
