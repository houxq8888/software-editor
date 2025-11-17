#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentFile("")
    , m_isModified(false)
    , m_featuresModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->featuresListView->setModel(m_featuresModel);
    ui->featuresListView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Set header for features list
    m_featuresModel->setHorizontalHeaderLabels({"功能特性"});

    // Connect signals
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->versionLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->developerLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->categoryLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->websiteLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->iconPathLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->screenshotPathLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->descriptionTextEdit, &QTextEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->featureNameLineEdit, &QLineEdit::textChanged, this, [this]() { setModified(true); });
    connect(ui->featureDescriptionTextEdit, &QTextEdit::textChanged, this, [this]() { setModified(true); });

    clearProductData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    if (saveChanges()) {
        clearProductData();
        m_currentFile = "";
        setModified(false);
        setWindowTitle("产品编辑器 - 新建");
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if (saveChanges()) {
        QString fileName = QFileDialog::getOpenFileName(this, "打开产品文件", "", "JSON文件 (*.json)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly)) {
                QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
                file.close();

                if (jsonDoc.isObject()) {
                    Product product;
                    if (product.fromJson(jsonDoc.object())) {
                        loadProductData(product);
                        m_currentFile = fileName;
                        setModified(false);
                        setWindowTitle(QString("产品编辑器 - %1").arg(QFileInfo(fileName).fileName()));
                    }
                }
            }
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (m_currentFile.isEmpty()) {
        on_actionSave_As_triggered();
    } else {
        saveProductData();
        setModified(false);
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存产品文件", "", "JSON文件 (*.json)");
    if (!fileName.isEmpty()) {
        m_currentFile = fileName;
        saveProductData();
        setModified(false);
        setWindowTitle(QString("产品编辑器 - %1").arg(QFileInfo(fileName).fileName()));
    }
}

void MainWindow::on_actionExit_triggered()
{
    if (saveChanges()) {
        close();
    }
}

void MainWindow::on_iconBrowseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择图标文件", "", "图像文件 (*.png *.jpg *.jpeg *.ico)");
    if (!fileName.isEmpty()) {
        ui->iconPathLineEdit->setText(fileName);
    }
}

void MainWindow::on_screenshotBrowseButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择截图文件", "", "图像文件 (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        ui->screenshotPathLineEdit->setText(fileName);
    }
}

void MainWindow::on_addFeatureButton_clicked()
{
    ProductFeature feature = getCurrentFeatureFromEditors();
    if (!feature.name.isEmpty()) {
        m_product.addFeature(feature);
        updateFeaturesList();
        ui->featureNameLineEdit->clear();
        ui->featureDescriptionTextEdit->clear();
        setModified(true);
    }
}

void MainWindow::on_removeFeatureButton_clicked()
{
    QModelIndex index = ui->featuresListView->currentIndex();
    if (index.isValid()) {
        int row = index.row();
        m_product.removeFeature(row);
        m_featuresModel->removeRow(row);
        setModified(true);
    }
}

void MainWindow::on_featuresListView_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        int row = index.row();
        if (row >= 0 && row < m_product.features().size()) {
            const ProductFeature &feature = m_product.features().at(row);
            ui->featureNameLineEdit->setText(feature.name);
            ui->featureDescriptionTextEdit->setText(feature.description);
        }
    }
}

void MainWindow::clearProductData()
{
    ui->nameLineEdit->clear();
    ui->versionLineEdit->clear();
    ui->developerLineEdit->clear();
    ui->categoryLineEdit->clear();
    ui->websiteLineEdit->clear();
    ui->iconPathLineEdit->clear();
    ui->screenshotPathLineEdit->clear();
    ui->descriptionTextEdit->clear();
    ui->featureNameLineEdit->clear();
    ui->featureDescriptionTextEdit->clear();
    m_featuresModel->clear();
    m_featuresModel->setHorizontalHeaderLabels({"功能特性"});
    m_product = Product();
}

void MainWindow::loadProductData(const Product &product)
{
    m_product = product;

    ui->nameLineEdit->setText(product.name());
    ui->versionLineEdit->setText(product.version());
    ui->developerLineEdit->setText(product.developer());
    ui->categoryLineEdit->setText(product.category());
    ui->websiteLineEdit->setText(product.website());
    ui->iconPathLineEdit->setText(product.iconPath());
    ui->screenshotPathLineEdit->setText(product.screenshotPath());
    ui->descriptionTextEdit->setText(product.description());

    updateFeaturesList();
}

void MainWindow::saveProductData()
{
    // Save current feature if any
    ProductFeature feature = getCurrentFeatureFromEditors();
    if (!feature.name.isEmpty() && ui->featuresListView->currentIndex().isValid()) {
        int row = ui->featuresListView->currentIndex().row();
        if (row >= 0 && row < m_product.features().size()) {
            QList<ProductFeature> features = m_product.features();
            features[row] = feature;
            m_product.setFeatures(features);
        }
    }

    // Update product data from UI
    m_product.setName(ui->nameLineEdit->text());
    m_product.setVersion(ui->versionLineEdit->text());
    m_product.setDeveloper(ui->developerLineEdit->text());
    m_product.setCategory(ui->categoryLineEdit->text());
    m_product.setWebsite(ui->websiteLineEdit->text());
    m_product.setIconPath(ui->iconPathLineEdit->text());
    m_product.setScreenshotPath(ui->screenshotPathLineEdit->text());
    m_product.setDescription(ui->descriptionTextEdit->toPlainText());

    // Write to file
    QFile file(m_currentFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument jsonDoc(m_product.toJson());
        file.write(jsonDoc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void MainWindow::setModified(bool modified)
{
    m_isModified = modified;
    QString windowTitle = m_currentFile.isEmpty() ? "产品编辑器 - 新建" : QString("产品编辑器 - %1").arg(QFileInfo(m_currentFile).fileName());
    if (modified) {
        windowTitle += " *";
    }
    setWindowTitle(windowTitle);
}

bool MainWindow::saveChanges()
{
    if (m_isModified) {
        QMessageBox::StandardButton button = QMessageBox::question(this, "保存更改", "是否保存当前编辑的产品信息？");
        if (button == QMessageBox::Save) {
            on_actionSave_triggered();
            return true;
        } else if (button == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::updateFeaturesList()
{
    m_featuresModel->clear();
    m_featuresModel->setHorizontalHeaderLabels({"功能特性"});

    for (const auto &feature : m_product.features()) {
        QStandardItem *item = new QStandardItem(feature.name);
        item->setData(feature.description, Qt::UserRole + 1);
        m_featuresModel->appendRow(item);
    }
}

ProductFeature MainWindow::getCurrentFeatureFromEditors() const
{
    ProductFeature feature;
    feature.name = ui->featureNameLineEdit->text();
    feature.description = ui->featureDescriptionTextEdit->toPlainText();
    return feature;
}