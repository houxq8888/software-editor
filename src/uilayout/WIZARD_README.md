# 向导功能使用说明

## 概述

向导功能（Wizard）是一个用于创建类似MFC Wizard的向导界面的功能模块，允许开发者创建多步骤的交互流程，引导用户完成复杂的任务。

## 核心类

### WizardPage
表示向导的单个页面，包含以下属性：
- `pageId`: 页面唯一标识符
- `title`: 页面标题
- `description`: 页面描述
- `uiInterface`: 关联的UI界面
- `isStartPage`: 是否为起始页面
- `isFinalPage`: 是否为最终页面
- `nextPageId`: 下一页ID
- `previousPageId`: 上一页ID
- `completionAction`: 完成动作
- `cancelAction`: 取消动作
- `enableNextButton`: 是否启用下一步按钮
- `enableBackButton`: 是否启用上一步按钮
- `enableFinishButton`: 是否启用完成按钮
- `enableCancelButton`: 是否启用取消按钮

### Wizard
管理向导流程的核心类，提供以下功能：
- 页面管理：添加、删除、查找页面
- 向导控制：启动、前进、后退、完成、取消
- 页面验证：支持为每个页面添加验证器
- 导航控制：启用/禁用导航按钮
- 事件处理：提供丰富的信号和槽机制
- 序列化/反序列化：支持将向导配置保存到文件或从文件加载

### WizardManager
管理多个向导实例的类，提供以下功能：
- 创建、删除、查找向导
- 管理当前活动的向导
- 序列化/反序列化：支持将所有向导配置保存到文件或从文件加载

## 集成到现有系统

向导功能已经集成到现有的状态机系统中，可以通过以下步骤使用：

1. 创建UIInterfaceManager实例
2. 创建StateMachineManager实例，并设置UIInterfaceManager
3. 创建WizardManager实例，并设置到StateMachineManager中
4. 创建向导和向导页面
5. 启动向导

## 使用示例

以下是一个简单的使用示例：

```cpp
// 初始化UI管理器和状态机管理器
UIInterfaceManager* uiManager = new UIInterfaceManager();
StateMachineManager* stateMachineManager = new StateMachineManager();
stateMachineManager->setUiInterfaceManager(uiManager);

// 初始化向导管理器
WizardManager* wizardManager = new WizardManager();
stateMachineManager->setWizardManager(wizardManager);

// 创建向导
Wizard* wizard = wizardManager->createWizard("示例向导", "这是一个演示向导功能的示例");

// 创建向导页面
WizardPage page1;
page1.pageId = "page1";
page1.title = "欢迎使用向导";
page1.description = "这是向导的第一步";
page1.uiInterface = createUIInterface("page1"); // 创建UI界面
page1.isStartPage = true;
page1.nextPageId = "page2";
wizard->addPage(page1);

// 创建其他页面...

// 启动向导
if (wizard->start()) {
    qDebug() << "向导已启动";
}

// 连接向导信号
connect(wizard, &Wizard::finished, [](Wizard::WizardResult result) {
    if (result == Wizard::WizardResult::Finished) {
        qDebug() << "向导已完成";
    } else if (result == Wizard::WizardResult::Canceled) {
        qDebug() << "向导已取消";
    }
});
```

## 导航控制

向导提供了以下导航方法：

```cpp
// 前进到下一页
bool goToNextPage();

// 返回到上一页
bool goToPreviousPage();

// 跳转到指定页面
bool goToPage(const QString &pageId);

// 完成向导
void finish();

// 取消向导
void cancel();
```

## 页面验证

可以为每个页面添加验证器，确保用户输入的信息有效：

```cpp
// 为页面1添加验证器
wizard->setPageValidator("page1", []() {
    // 执行验证逻辑
    bool isValid = ...; // 验证用户输入
    return isValid;
});
```

## 序列化

向导支持将配置保存到文件或从文件加载：

```cpp
// 保存向导配置到文件
wizardManager->saveToFile("wizards.json");

// 从文件加载向导配置
wizardManager->loadFromFile("wizards.json");
```

## 事件信号

向导提供了丰富的信号，可以用于响应向导的各种状态变化：

- `started()`: 向导已启动
- `finished(WizardResult result)`: 向导已完成
- `canceled()`: 向导已取消
- `pageChanged(const WizardPage &previousPage, const WizardPage &currentPage)`: 页面已切换
- `nextButtonClicked()`: 下一步按钮被点击
- `previousButtonClicked()`: 上一步按钮被点击
- `finishButtonClicked()`: 完成按钮被点击
- `cancelButtonClicked()`: 取消按钮被点击
- `navigationEnabledChanged(bool canGoNext, bool canGoPrevious, bool canFinish, bool canCancel)`: 导航按钮状态已改变

## 与状态机集成

向导功能已经与现有的状态机系统集成，可以通过以下方式使用：

```cpp
// 在状态机管理器中获取向导管理器
WizardManager* wizardManager = stateMachineManager->wizardManager();

// 创建向导并与状态机关联
Wizard* wizard = wizardManager->createWizard("状态机向导", "与状态机关联的向导");
wizard->setStateMachineManager(stateMachineManager);
```

## 示例应用程序

在 `wizardexample.cpp` 文件中提供了一个完整的示例应用程序，演示了如何使用向导功能。

## 编译和运行

向导功能已经集成到项目中，可以通过以下命令编译：

```bash
./build.bat
```

编译完成后，可以运行生成的可执行文件：

```bash
./build/software-editor.exe
```

## 注意事项

1. 向导页面的UI界面需要提前创建好，并通过 `uiInterface` 属性关联到向导页面。
2. 向导页面的导航逻辑可以通过设置 `nextPageId` 和 `previousPageId` 属性来控制，也可以通过代码动态计算。
3. 页面验证器应该返回一个布尔值，表示页面是否验证通过。
4. 向导功能支持序列化，可以将向导配置保存到文件或从文件加载，方便复用和共享。
5. 向导功能已经与现有的状态机系统集成，可以与状态机一起使用，实现更复杂的交互逻辑。
