#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QMainWindow>
namespace Ui {
class MainWindow;
class IDItem;
class HashItem;
}

class MainWindow;
class IDItem;
class HashItem;
typedef QSharedPointer<QIODevice> InputPtr;
typedef QSharedPointer<QStringList> QStringList_ptr;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSettings& settings() {
        return settings_;
    }

    bool keepSearching() const {
        return keepSearching_;
    }

    bool useBase32() const {
        return useBase32_;
    }

public slots:
    void on_searchButton_clicked();
    void on_stopButton_clicked();
    void on_patternLineEdit_returnPressed();
    void on_unpackAction_triggered();
    void on_exitAction_triggered();
    void on_selectAction_triggered();
    void on_base32Action_triggered(bool base32);
    void on_cp1251Action_triggered(bool cp1251);
    void on_selectDescriptionAction_triggered();

    void openUrl(QString url);

private slots:
    void addLines(QStringList_ptr lines);
    void startFilling();
    void stopFilling();
    void showDescription(int id);
    void rowChanged(QModelIndex current);
    void search();

    void on_action_copy_rutracker_link_triggered();

    void on_action_open_rutracker_triggered();

    void on_action_copy_magnet_triggered();

    void on_action_open_magnet_triggered();

private:
    Ui::MainWindow *ui;
    bool keepSearching_;
    QSettings settings_;
    bool useBase32_;

    void setData(int row, int col, const QVariant& data);
    QIODevice* getInputDevice();
    void updateButtons();
    QString descriptionById(int id);
    IDItem* get_current_item();
    HashItem* get_current_hash_item();
};

class SearchingThread : public QObject, public QRunnable {
    Q_OBJECT
public:
    SearchingThread(QIODevice* input, MainWindow* window,
                    int limit, QString pattern, bool cp1251);
    void run();
signals:
    void newLines(QStringList_ptr lines);
    void stopFilling();
private:
    QIODevice* input_;
    MainWindow* window_;
    int limit_;
    QString pattern_;
    bool cp1251_;
};

#endif // MAINWINDOW_H
