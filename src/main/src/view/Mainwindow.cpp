#include "view/Mainwindow.h"

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QCompleter>
#include <QDateTime>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QJsonArray>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QScreen>
#include <QSet>
#include <QShortcut>
#include <QStringListModel>
#include <QTextStream>
#include <QVBoxLayout>

#include "./ui_Mainwindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), totalCheckins(0) {
    ui->setupUi(this);

    setupUI();
    loadStyleSheet();
    loadHistoryDatabase();
    updateCompleters();

    // 初始化 RST 默认值
    leRST->setText("59");
}

MainWindow::~MainWindow() {
    saveHistoryDatabase();
    delete ui;
}

void MainWindow::setupUI() {
    setWindowTitle("Radio Net Record Tool - ICOM/FT-710 Style");
    resize(1024, 768);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // --- 台网基础信息区 ---
    QGridLayout* netInfoLayout = new QGridLayout();
    leNetName = new QLineEdit();
    leNetName->setText(QString::fromUtf8("深圳梧桐山/439.4625MHz业余中继台网签到记录表"));
    leNetName->setStyleSheet("font-size: 16px; font-weight: bold; color: #FF8C00;");

    leNetControl = new QLineEdit();
    leNetControl->setPlaceholderText(QString::fromUtf8("例如: BD7IJR / BI7IZD"));

    leNetDate = new QLineEdit(QDateTime::currentDateTime().toString("yyyy.MM.dd"));
    leNetTime = new QLineEdit(QString::fromUtf8("20:00-22:00"));

    netInfoLayout->addWidget(new QLabel(QString::fromUtf8("台网名称:")), 0, 0);
    netInfoLayout->addWidget(leNetName, 0, 1, 1, 5);  // 跨列

    netInfoLayout->addWidget(new QLabel(QString::fromUtf8("主控呼号:")), 1, 0);
    netInfoLayout->addWidget(leNetControl, 1, 1);
    netInfoLayout->addWidget(new QLabel(QString::fromUtf8("日期:")), 1, 2);
    netInfoLayout->addWidget(leNetDate, 1, 3);
    netInfoLayout->addWidget(new QLabel(QString::fromUtf8("时间:")), 1, 4);
    netInfoLayout->addWidget(leNetTime, 1, 5);

    mainLayout->addLayout(netInfoLayout);

    // 分隔线
    QFrame* hLine = new QFrame();
    hLine->setFrameShape(QFrame::HLine);
    hLine->setStyleSheet("background-color: #3A3A3A;");
    mainLayout->addWidget(hLine);

    // --- 顶部表单区 ---
    QGridLayout* formLayout = new QGridLayout();

    leCallsign = new QLineEdit();
    leCallsign->setPlaceholderText("CALLSIGN");
    leRST = new QLineEdit();
    cbQTH = new QComboBox();
    cbQTH->setEditable(true);
    if (cbQTH->completer()) {
        cbQTH->completer()->setFilterMode(Qt::MatchContains);
    }
    lePower = new QLineEdit();
    cbRig = new QComboBox();
    cbRig->setEditable(true);
    if (cbRig->completer()) {
        cbRig->completer()->setFilterMode(Qt::MatchContains);
    }
    cbAntenna = new QComboBox();
    cbAntenna->setEditable(true);
    if (cbAntenna->completer()) {
        cbAntenna->completer()->setFilterMode(Qt::MatchContains);
    }
    leTopic = new QLineEdit();
    leRemarks = new QLineEdit();

    formLayout->addWidget(new QLabel("Callsign (呼号):"), 0, 0);
    formLayout->addWidget(leCallsign, 0, 1);
    formLayout->addWidget(new QLabel("RST (信号):"), 0, 2);
    formLayout->addWidget(leRST, 0, 3);
    formLayout->addWidget(new QLabel("QTH (位置):"), 0, 4);
    formLayout->addWidget(cbQTH, 0, 5);

    formLayout->addWidget(new QLabel("Power (功率):"), 1, 0);
    formLayout->addWidget(lePower, 1, 1);
    formLayout->addWidget(new QLabel("Rig (设备):"), 1, 2);
    formLayout->addWidget(cbRig, 1, 3);
    formLayout->addWidget(new QLabel("Antenna (天线):"), 1, 4);
    formLayout->addWidget(cbAntenna, 1, 5);

    formLayout->addWidget(new QLabel("Topic (话题):"), 2, 0);
    formLayout->addWidget(leTopic, 2, 1, 1, 3);
    formLayout->addWidget(new QLabel("Remarks (备注):"), 2, 4);
    formLayout->addWidget(leRemarks, 2, 5);

    mainLayout->addLayout(formLayout);

    // --- 按钮区 ---
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnSubmit = new QPushButton(QString::fromUtf8("添加记录 (Enter)"));
    btnAddControl = new QPushButton(QString::fromUtf8("插入主控记录"));
    btnAddControl->setStyleSheet("background-color: #D4AF37; color: #000000;");
    btnImport = new QPushButton(QString::fromUtf8("导入历史"));
    btnExportPNG = new QPushButton(QString::fromUtf8("导出长图"));
    btnExportCSV = new QPushButton(QString::fromUtf8("导出 CSV"));

    btnLayout->addWidget(btnSubmit);
    btnLayout->addWidget(btnAddControl);
    btnLayout->addWidget(btnImport);
    btnLayout->addWidget(btnExportPNG);
    btnLayout->addWidget(btnExportCSV);
    mainLayout->addLayout(btnLayout);

    // --- 表格区 ---
    tableWidget = new QTableWidget(0, 9);
    tableWidget->setHorizontalHeaderLabels(
        { "Time", "Callsign", "RST", "QTH", "Power", "Rig", "Antenna", "Topic", "Remarks" });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setAlternatingRowColors(false);
    tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mainLayout->addWidget(tableWidget);

    // --- 状态栏 ---
    lblStatus = new QLabel("Ready | Check-ins: 0");
    statusBar()->addWidget(lblStatus);

    // --- 信号与槽连接 ---
    connect(leCallsign, &QLineEdit::textChanged, this, &MainWindow::onCallsignTextChanged);
    connect(leCallsign, &QLineEdit::editingFinished, this, &MainWindow::onCallsignEditingFinished);

    // “全局回车即记录” 逻辑：使用 QShortcut 捕捉全局 Enter 键
    QShortcut* shortcutReturn = new QShortcut(QKeySequence(Qt::Key_Return), this);
    QShortcut* shortcutEnter = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    connect(shortcutReturn, &QShortcut::activated, this, &MainWindow::onSubmitRecord);
    connect(shortcutEnter, &QShortcut::activated, this, &MainWindow::onSubmitRecord);
    connect(btnSubmit, &QPushButton::clicked, this, &MainWindow::onSubmitRecord);
    connect(btnAddControl, &QPushButton::clicked, this, &MainWindow::onAddControlRecord);

    connect(btnImport, &QPushButton::clicked, this, &MainWindow::onImportHistory);
    connect(btnExportPNG, &QPushButton::clicked, this, &MainWindow::onExportPNG);
    connect(btnExportCSV, &QPushButton::clicked, this, &MainWindow::onExportCSV);
    connect(tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    // 为下拉框添加事件过滤器以捕捉 Delete 键
    cbQTH->view()->installEventFilter(this);
    cbRig->view()->installEventFilter(this);
    cbAntenna->view()->installEventFilter(this);
	
}

// 拦截按键事件（允许下拉列表中按 Delete 键删除条目）
bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Delete) {
            if (watched == cbQTH->view()) {
                int row = cbQTH->view()->currentIndex().row();
                if (row >= 0)
                    cbQTH->removeItem(row);
                return true;
            } else if (watched == cbRig->view()) {
                int row = cbRig->view()->currentIndex().row();
                if (row >= 0)
                    cbRig->removeItem(row);
                return true;
            } else if (watched == cbAntenna->view()) {
                int row = cbAntenna->view()->currentIndex().row();
                if (row >= 0)
                    cbAntenna->removeItem(row);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

// 自动大写转换
void MainWindow::onCallsignTextChanged(const QString& text) {
    leCallsign->blockSignals(true);
    int cursor = leCallsign->cursorPosition();
    leCallsign->setText(text.toUpper());
    leCallsign->setCursorPosition(cursor);
    leCallsign->blockSignals(false);

    // 实时检测，一旦匹配到完整呼号立刻补全，极大提升体验
    onCallsignEditingFinished();
}

// 智能自动补全逻辑
void MainWindow::onCallsignEditingFinished() {
    QString call = leCallsign->text().trimmed().toUpper();
    if (call.isEmpty())
        return;

    if (historyDb.contains(call)) {
        StationInfo info = historyDb.value(call);
        if (cbQTH->currentText().trimmed().isEmpty())
            cbQTH->setCurrentText(info.qth);
        if (cbRig->currentText().trimmed().isEmpty())
            cbRig->setCurrentText(info.rig);
        if (cbAntenna->currentText().trimmed().isEmpty())
            cbAntenna->setCurrentText(info.antenna);
        if (lePower->text().trimmed().isEmpty())
            lePower->setText(info.power);
        lblStatus->setText(QString("Auto-filled profile for %1").arg(call));
    }
}

// 提交记录的核心功能
void MainWindow::onSubmitRecord() {
    QString call = leCallsign->text().trimmed().toUpper();
    if (call.isEmpty())
        return;

    // 如果是通过全局快捷键触发的提交，此时尚未失去焦点，可能补全仍未发生，因此强制补全一次
    onCallsignEditingFinished();

    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");

    // 列表居中显示 Helper
    auto createCenteredItem = [](const QString& text) {
        QTableWidgetItem* item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        return item;
    };

    QString powerText = lePower->text().trimmed();
    if (!powerText.isEmpty() && !powerText.endsWith("W", Qt::CaseInsensitive)) {
        powerText += "W";
    }

    // 1. 添加到 TableWidget
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    tableWidget->setItem(row, 0, createCenteredItem(timestamp));
    tableWidget->setItem(row, 1, createCenteredItem(call));
    tableWidget->setItem(row, 2, createCenteredItem(leRST->text()));
    tableWidget->setItem(row, 3, createCenteredItem(cbQTH->currentText()));
    tableWidget->setItem(row, 4, createCenteredItem(powerText));
    tableWidget->setItem(row, 5, createCenteredItem(cbRig->currentText()));
    tableWidget->setItem(row, 6, createCenteredItem(cbAntenna->currentText()));
    tableWidget->setItem(row, 7, createCenteredItem(leTopic->text()));
    tableWidget->setItem(row, 8, createCenteredItem(leRemarks->text()));

    // 2. 更新历史档案库并持久化，同时增加下拉选择记录
    QString qthText = cbQTH->currentText();
    QString rigText = cbRig->currentText();
    QString antText = cbAntenna->currentText();

    if (!qthText.isEmpty() && cbQTH->findText(qthText) == -1)
        cbQTH->addItem(qthText);
    if (!rigText.isEmpty() && cbRig->findText(rigText) == -1)
        cbRig->addItem(rigText);
    if (!antText.isEmpty() && cbAntenna->findText(antText) == -1)
        cbAntenna->addItem(antText);

    StationInfo si{ qthText, rigText, antText, powerText };
    historyDb.insert(call, si);
    saveHistoryDatabase();  // 实时保存更新

    // 3. 自动追加记录防掉电
    LogEntry entry{ timestamp, call,    leRST->text(),   qthText,          powerText,
                    rigText,   antText, leTopic->text(), leRemarks->text() };
    currentLogs.append(entry);
    autoSaveLog(entry);

    // 4. 清理输入并让焦点归位 (回车即记录核心体验)
    leCallsign->clear();
    cbQTH->clearEditText();
    lePower->clear();
    cbRig->clearEditText();
    cbAntenna->clearEditText();
    leTopic->clear();
    leRemarks->clear();
    leRST->setText("59");
    leCallsign->setFocus();  // 焦点回到呼号

    totalCheckins++;
    updateStatusBar();
}

// 插入中途切换的主控记录
void MainWindow::onAddControlRecord() {
    QString call = leCallsign->text().trimmed().toUpper();
    if (call.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先在呼号框输入新的主控呼号"));
        return;
    }

    bool ok;
    QString stage = QInputDialog::getText(this, QString::fromUtf8("添加主控记录"),
                                          QString::fromUtf8("请输入阶段标题 (如: 下半场、特邀主控 等):"),
                                          QLineEdit::Normal, QString::fromUtf8("下半场"), &ok);
    if (!ok || stage.isEmpty())
        return;

    // 确保补全触发更新一次缓存
    onCallsignEditingFinished();

    QString powerText = lePower->text().trimmed();
    if (!powerText.isEmpty() && !powerText.endsWith("W", Qt::CaseInsensitive)) {
        powerText += "W";
    }

    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    QFont boldFont;
    boldFont.setBold(true);
    QColor bgColor("#FFFF00");  // 亮黄色背景
    QColor fgColor("#000000");  // 纯黑字体

    auto createControlItem = [&](const QString& text) {
        QTableWidgetItem* item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        item->setBackground(bgColor);
        item->setForeground(fgColor);
        item->setFont(boldFont);
        return item;
    };

    // Time 处或者 Topic 处放阶段名，可以放在第一个空或者专门跨列，由于 QTableWidget
    // 不适合强行跨多行导出，我们按照栏目强行分配
    tableWidget->setItem(row, 0, createControlItem(stage));  // 第一列放阶段名 (代替Time)
    tableWidget->setItem(row, 1, createControlItem(call));   // 呼号
    tableWidget->setItem(row, 2, createControlItem(""));     // RST空
    tableWidget->setItem(row, 3, createControlItem(cbQTH->currentText()));
    tableWidget->setItem(row, 4, createControlItem(powerText));
    tableWidget->setItem(row, 5, createControlItem(cbRig->currentText()));
    tableWidget->setItem(row, 6, createControlItem(cbAntenna->currentText()));
    tableWidget->setItem(row, 7, createControlItem(leTopic->text()));
    tableWidget->setItem(row, 8, createControlItem(leRemarks->text()));

    // 智能更新到全局当值主控基础信息字段
    QString currentCtrl = leNetControl->text();
    if (!currentCtrl.contains(call)) {
        if (currentCtrl.isEmpty())
            leNetControl->setText(call);
        else
            leNetControl->setText(currentCtrl + " / " + call);
    }

    // 历史档案与防掉电保存
    QString qthText = cbQTH->currentText();
    QString rigText = cbRig->currentText();
    QString antText = cbAntenna->currentText();

    if (!qthText.isEmpty() && cbQTH->findText(qthText) == -1)
        cbQTH->addItem(qthText);
    if (!rigText.isEmpty() && cbRig->findText(rigText) == -1)
        cbRig->addItem(rigText);
    if (!antText.isEmpty() && cbAntenna->findText(antText) == -1)
        cbAntenna->addItem(antText);

    StationInfo si{ qthText, rigText, antText, powerText };
    historyDb.insert(call, si);
    saveHistoryDatabase();

    LogEntry entry{ stage, call, "CTRL", qthText, powerText, rigText, antText, leTopic->text(), leRemarks->text() };
    currentLogs.append(entry);
    autoSaveLog(entry);

    // 清理以备下一步输入
    leCallsign->clear();
    cbQTH->clearEditText();
    lePower->clear();
    cbRig->clearEditText();
    cbAntenna->clearEditText();
    leTopic->clear();
    leRemarks->clear();
    leRST->setText("59");
    leCallsign->setFocus();
}

// 本地启动读取历史 JSON
void MainWindow::loadHistoryDatabase() {
    QFile file("history_database.json");
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();

    QSet<QString> qthSet, rigSet, antSet;

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        QJsonObject stObj = it.value().toObject();
        StationInfo info;
        info.qth = stObj["qth"].toString();
        info.rig = stObj["rig"].toString();
        info.antenna = stObj["antenna"].toString();
        info.power = stObj["power"].toString();
        historyDb.insert(it.key(), info);

        if (!info.qth.isEmpty())
            qthSet.insert(info.qth);
        if (!info.rig.isEmpty())
            rigSet.insert(info.rig);
        if (!info.antenna.isEmpty())
            antSet.insert(info.antenna);
    }

    cbQTH->addItems(qthSet.values());
    cbRig->addItems(rigSet.values());
    cbAntenna->addItems(antSet.values());
    cbQTH->setCurrentText("");
    cbRig->setCurrentText("");
    cbAntenna->setCurrentText("");
    updateCompleters();
}

// 保存历史库到 JSON
void MainWindow::saveHistoryDatabase() {
    QJsonObject rootObj;
    for (auto it = historyDb.begin(); it != historyDb.end(); ++it) {
        QJsonObject stObj;
        stObj["qth"] = it.value().qth;
        stObj["rig"] = it.value().rig;
        stObj["antenna"] = it.value().antenna;
        stObj["power"] = it.value().power;
        rootObj.insert(it.key(), stObj);
    }
    QJsonDocument doc(rootObj);
    QFile file("history_database.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    }
    updateCompleters();
}

void MainWindow::updateCompleters() {
    QStringList calls = historyDb.keys();

    // 更新 Callsign 的补全器
    if (!leCallsign->completer()) {
        QCompleter* comp = new QCompleter(calls, this);
        comp->setCaseSensitivity(Qt::CaseInsensitive);
        comp->setFilterMode(Qt::MatchContains);
        leCallsign->setCompleter(comp);
    } else {
        auto model = qobject_cast<QStringListModel*>(leCallsign->completer()->model());
        if (model) {
            model->setStringList(calls);
        } else {
            leCallsign->completer()->deleteLater();
            QCompleter* comp = new QCompleter(calls, this);
            comp->setCaseSensitivity(Qt::CaseInsensitive);
            comp->setFilterMode(Qt::MatchContains);
            leCallsign->setCompleter(comp);
        }
    }
}

// 增量防掉电备份 (CSV)
void MainWindow::autoSaveLog(const LogEntry& entry) {
    QFile file("current_net_backup.csv");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << entry.time << "," << entry.callsign << "," << entry.rst << "," << entry.qth << "\n";
    }
}

// 导入外部CSV补全历史
void MainWindow::onImportHistory() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import History CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int count = 0;
        while (!in.atEnd()) {
            QStringList fields = in.readLine().split(",");
            if (fields.size() >= 5) {
                QString call = fields[0].trimmed().toUpper();
                StationInfo info{ fields[1], fields[2], fields[3], fields[4] };
                historyDb.insert(call, info);
                count++;
            }
        }
        saveHistoryDatabase();
        QMessageBox::information(this, "Success", QString("Imported %1 history records.").arg(count));
    }
}

// 渲染 QTableWidget 和数据为高清长图
void MainWindow::onExportPNG() {
    QString defaultName = QDateTime::currentDateTime().toString("yyyy-MM-dd ") + leNetName->text() + ".png";
    // 替换掉文件名中可能不合法的字符（如果名称里有斜杠等字符，文件系统会拒绝）
    defaultName.replace("/", "-");

    QString fileName = QFileDialog::getSaveFileName(this, "Export Dashboard", defaultName, "PNG Images (*.png)");
    if (fileName.isEmpty())
        return;

    int padding = 20;
    int titleHeight = 120;  // 为新的丰富表头预留空间
    int tableHeight =
        tableWidget->horizontalHeader()->height() + tableWidget->rowHeight(0) * tableWidget->rowCount() + 20;
    tableWidget->clearSelection();

    int totalWidth = tableWidget->width() + padding * 2;
    QPixmap pixmap(totalWidth, titleHeight + tableHeight + padding * 2);
    pixmap.fill(QColor("#111111"));  // 和 QSS 底部背景一致

    QPainter painter(&pixmap);
    painter.setPen(QColor("#FFFFFF"));
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制核心表格名称标题 (大字号居中)
    QFont titleFont = this->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    QRect titleRect(padding, padding + 10, tableWidget->width(), 40);
    painter.drawText(titleRect, Qt::AlignCenter, leNetName->text());

    // 绘制底部信息边框和子信息 (仿表格框线)
    painter.setPen(QPen(QColor("#555555"), 2));
    int line1Y = padding + 60;
    int line2Y = padding + 90;
    int line3Y = padding + 120;
    painter.drawLine(padding, line1Y, totalWidth - padding, line1Y);
    painter.drawLine(padding, line2Y, totalWidth - padding, line2Y);
    painter.drawLine(padding, line3Y, totalWidth - padding, line3Y);

    // 垂直分割线
    painter.drawLine(padding + tableWidget->width() / 2, line1Y, padding + tableWidget->width() / 2, line3Y);

    // 填写子信息文字
    painter.setPen(QColor("#DCDCDC"));
    QFont subFont = this->font();
    subFont.setPointSize(12);
    subFont.setBold(false);
    painter.setFont(subFont);

    // 第一行: 主控 / 日期
    QRect ctrlRect(padding + 10, line1Y, tableWidget->width() / 2 - 20, 30);
    painter.drawText(ctrlRect, Qt::AlignLeft | Qt::AlignVCenter,
                     QString::fromUtf8("当值主控: ") + leNetControl->text());

    QRect dateRect(padding + tableWidget->width() / 2 + 10, line1Y, tableWidget->width() / 2 - 20, 30);
    painter.drawText(dateRect, Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8("日期: ") + leNetDate->text());

    // 第二行: 总签到 / 时间
    QRect totalRect(padding + 10, line2Y, tableWidget->width() / 2 - 20, 30);
    painter.drawText(totalRect, Qt::AlignLeft | Qt::AlignVCenter,
                     QString::fromUtf8("总签到数: %1 人").arg(totalCheckins));

    QRect timeRect(padding + tableWidget->width() / 2 + 10, line2Y, tableWidget->width() / 2 - 20, 30);
    painter.drawText(timeRect, Qt::AlignLeft | Qt::AlignVCenter, QString::fromUtf8("时间: ") + leNetTime->text());

    // 恢复坐标系将表格渲染进Pixmap
    painter.translate(padding, titleHeight + padding);
    tableWidget->render(&painter);
    painter.end();

    pixmap.save(fileName);
    QMessageBox::information(this, "Success", "Image Exported Successfully!");
}

void MainWindow::onExportCSV() {
    QString defaultName = QDateTime::currentDateTime().toString("yyyy-MM-dd ") + leNetName->text() + ".csv";
    defaultName.replace("/", "-");

    QString fileName = QFileDialog::getSaveFileName(this, "Export CSV", defaultName, "CSV Files (*.csv)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Time,Callsign,RST,QTH,Power,Rig,Antenna,Topic,Remarks\n";
        for (const LogEntry& entry : currentLogs) {
            out << entry.time << "," << entry.callsign << "," << entry.rst << "," << entry.qth << "," << entry.power
                << "," << entry.rig << "," << entry.antenna << "," << entry.topic << "," << entry.remarks << "\n";
        }
        QMessageBox::information(this, "Success", "CSV Exported Successfully!");
    }
}

void MainWindow::showContextMenu(const QPoint& pos) {
    QMenu menu(this);
    QAction* delAction = menu.addAction("Delete Record");
    connect(delAction, &QAction::triggered, this, &MainWindow::deleteSelectedRow);
    menu.exec(tableWidget->viewport()->mapToGlobal(pos));
}

void MainWindow::deleteSelectedRow() {
    int row = tableWidget->currentRow();
    if (row >= 0) {
        // 主控记录不参与签到计数，只有普通记录才减少计数
        if (currentLogs[row].rst != "CTRL") {
            totalCheckins--;
        }

        tableWidget->removeRow(row);
        currentLogs.removeAt(row);
        updateStatusBar();
    }
}

void MainWindow::updateStatusBar() {
    lblStatus->setText(QString("Ready | Check-ins: %1").arg(totalCheckins));
}

// 加载高级暗黑频谱仪风格 QSS
void MainWindow::loadStyleSheet() {
    QString qss = R"(
        QMainWindow {
            background-color: #1A1A1A;
        }
        QLabel {
            color: #DCDCDC;
            font-size: 14px;
            font-weight: bold;
        }
        QLineEdit, QComboBox {
            background-color: #2b2b2b;
            color: #FFFFFF;
            border: 2px solid #3A3A3A;
            border-radius: 4px;
            padding: 6px;
            font-size: 14px;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 2px solid #FF8C00;
            background-color: #222222;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid #3A3A3A;
        }
        QComboBox::down-arrow {
            image: url("data:image/svg+xml;utf8,<svg width='12' height='12' viewBox='0 0 12 12' xmlns='http://www.w3.org/2000/svg'><path d='M2 4 L6 8 L10 4' stroke='white' stroke-width='2' fill='none'/></svg>");
        }
        QComboBox QAbstractItemView {
            background-color: #2b2b2b;
            color: #FFFFFF;
            selection-background-color: #FF8C00;
            border: 1px solid #3A3A3A;
        }
        QPushButton {
            background-color: #005F9E;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #0078D7;
        }
        QPushButton:pressed {
            background-color: #FF8C00;
        }
        QTableWidget {
            background-color: #1E1E1E;
            color: #DCDCDC;
            gridline-color: #333333;
            selection-background-color: #FF8C00;
            selection-color: #FFFFFF;
            font-size: 13px;
        }
        QHeaderView::section {
            background-color: #2b2b2b;
            color: #FF8C00;
            padding: 4px;
            border: 1px solid #333333;
            font-weight: bold;
        }
    )";
    qApp->setStyleSheet(qss);
}
