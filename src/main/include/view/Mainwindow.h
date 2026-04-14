#ifndef _MAINWINDOW_H__
#define _MAINWINDOW_H__

#include <QComboBox>
#include <QFile>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// 历史电台档案
struct StationInfo {
    QString qth;
    QString rig;
    QString antenna;
    QString power;
};

// 当前台网日志
struct LogEntry {
    QString time;
    QString callsign;
    QString rst;
    QString qth;
    QString power;
    QString rig;
    QString antenna;
    QString topic;
    QString remarks;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;  // 拦截下拉框删除事件

private slots:
    void onCallsignTextChanged(const QString& text);  // 自动大写转换
    void onCallsignEditingFinished();                 // 失去焦点/Tab触发自动补全
    void onSubmitRecord();                            // 提交记录
    void onAddControlRecord();                        // 添加主控切换记录
    void onImportHistory();                           // 导入外部CSV历史数据
    void onExportPNG();                               // 导出为长图
    void onExportCSV();                               // 导出日志CSV
    void showContextMenu(const QPoint& pos);          // 右键菜单
    void deleteSelectedRow();                         // 删除记录

private:
    void setupUI();
    void loadStyleSheet();
    void loadHistoryDatabase();
    void saveHistoryDatabase();
    void autoSaveLog(const LogEntry& entry);
    void updateStatusBar();

    Ui::MainWindow* ui;

    // UI 组件 - 台网基础信息
    QLineEdit* leNetName;
    QLineEdit* leNetControl;
    QLineEdit* leNetDate;
    QLineEdit* leNetTime;

    // UI 组件 - 记录表单
    QLineEdit* leCallsign;
    QLineEdit* leRST;
    QComboBox* cbQTH;
    QLineEdit* lePower;
    QComboBox* cbRig;
    QComboBox* cbAntenna;
    QLineEdit* leTopic;
    QLineEdit* leRemarks;
    QPushButton* btnSubmit;
    QPushButton* btnAddControl;
    QPushButton* btnImport;
    QPushButton* btnExportPNG;
    QPushButton* btnExportCSV;
    QTableWidget* tableWidget;
    QLabel* lblStatus;

    // 数据结构
    QHash<QString, StationInfo> historyDb;  // 历史数据库
    QList<LogEntry> currentLogs;            // 当前日志表
    int totalCheckins;
};

#endif  //  _MAINWINDOW_H__