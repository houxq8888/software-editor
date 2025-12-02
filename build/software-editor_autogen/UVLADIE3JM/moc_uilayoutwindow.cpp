/****************************************************************************
** Meta object code from reading C++ file 'uilayoutwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/uilayoutwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'uilayoutwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14UILayoutWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto UILayoutWindow::qt_create_metaobjectdata<qt_meta_tag_ZN14UILayoutWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "UILayoutWindow",
        "on_actionSave_Layout_triggered",
        "",
        "on_actionSave_As_Layout_triggered",
        "on_actionLoad_Layout_triggered",
        "on_actionUndo_triggered",
        "on_actionRedo_triggered",
        "on_widgetListWidget_itemDoubleClicked",
        "QTreeWidgetItem*",
        "item",
        "column",
        "on_actionNew_Layout_triggered",
        "on_actionPreview_triggered",
        "handleDoubleClick",
        "pos",
        "onHandleDragged",
        "handleIndex",
        "delta",
        "onHandleReleased",
        "onEditAreaDoubleClicked",
        "onBindFeatureButtonClicked",
        "onUnbindFeatureButtonClicked",
        "onSyncFeaturesButtonClicked",
        "onFeaturesTreeWidgetItemDoubleClicked",
        "updateStatusBarWithProductContext",
        "suggestLayoutForFeatures",
        "highlightRelevantWidgets",
        "featureName",
        "showFeatureContextMenu",
        "onActionSuggestLayoutTriggered",
        "onActionEventActionEditorTriggered",
        "updatePropertiesEditor",
        "QWidget*",
        "widget",
        "onPropertyItemChanged",
        "onInterfaceComboBoxChanged",
        "index",
        "onAddInterfaceAction",
        "onDeleteInterfaceAction",
        "onRenameInterfaceAction",
        "onCopyInterfaceAction"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'on_actionSave_Layout_triggered'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_actionSave_As_Layout_triggered'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_actionLoad_Layout_triggered'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_actionUndo_triggered'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_actionRedo_triggered'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_widgetListWidget_itemDoubleClicked'
        QtMocHelpers::SlotData<void(QTreeWidgetItem *, int)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 8, 9 }, { QMetaType::Int, 10 },
        }}),
        // Slot 'on_actionNew_Layout_triggered'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_actionPreview_triggered'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleDoubleClick'
        QtMocHelpers::SlotData<void(const QPoint &)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPoint, 14 },
        }}),
        // Slot 'onHandleDragged'
        QtMocHelpers::SlotData<void(int, const QPoint &)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 16 }, { QMetaType::QPoint, 17 },
        }}),
        // Slot 'onHandleReleased'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEditAreaDoubleClicked'
        QtMocHelpers::SlotData<void(const QPoint &)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPoint, 14 },
        }}),
        // Slot 'onBindFeatureButtonClicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUnbindFeatureButtonClicked'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSyncFeaturesButtonClicked'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFeaturesTreeWidgetItemDoubleClicked'
        QtMocHelpers::SlotData<void(QTreeWidgetItem *, int)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 8, 9 }, { QMetaType::Int, 10 },
        }}),
        // Slot 'updateStatusBarWithProductContext'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'suggestLayoutForFeatures'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'highlightRelevantWidgets'
        QtMocHelpers::SlotData<void(const QString &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 27 },
        }}),
        // Slot 'showFeatureContextMenu'
        QtMocHelpers::SlotData<void(const QPoint &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPoint, 14 },
        }}),
        // Slot 'onActionSuggestLayoutTriggered'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onActionEventActionEditorTriggered'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updatePropertiesEditor'
        QtMocHelpers::SlotData<void(QWidget *)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 32, 33 },
        }}),
        // Slot 'onPropertyItemChanged'
        QtMocHelpers::SlotData<void(QTreeWidgetItem *, int)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 8, 9 }, { QMetaType::Int, 10 },
        }}),
        // Slot 'onInterfaceComboBoxChanged'
        QtMocHelpers::SlotData<void(int)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 36 },
        }}),
        // Slot 'onAddInterfaceAction'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteInterfaceAction'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRenameInterfaceAction'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCopyInterfaceAction'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<UILayoutWindow, qt_meta_tag_ZN14UILayoutWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject UILayoutWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14UILayoutWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14UILayoutWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14UILayoutWindowE_t>.metaTypes,
    nullptr
} };

void UILayoutWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UILayoutWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->on_actionSave_Layout_triggered(); break;
        case 1: _t->on_actionSave_As_Layout_triggered(); break;
        case 2: _t->on_actionLoad_Layout_triggered(); break;
        case 3: _t->on_actionUndo_triggered(); break;
        case 4: _t->on_actionRedo_triggered(); break;
        case 5: _t->on_widgetListWidget_itemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 6: _t->on_actionNew_Layout_triggered(); break;
        case 7: _t->on_actionPreview_triggered(); break;
        case 8: _t->handleDoubleClick((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 9: _t->onHandleDragged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2]))); break;
        case 10: _t->onHandleReleased(); break;
        case 11: _t->onEditAreaDoubleClicked((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 12: _t->onBindFeatureButtonClicked(); break;
        case 13: _t->onUnbindFeatureButtonClicked(); break;
        case 14: _t->onSyncFeaturesButtonClicked(); break;
        case 15: _t->onFeaturesTreeWidgetItemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 16: _t->updateStatusBarWithProductContext(); break;
        case 17: _t->suggestLayoutForFeatures(); break;
        case 18: _t->highlightRelevantWidgets((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: _t->showFeatureContextMenu((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 20: _t->onActionSuggestLayoutTriggered(); break;
        case 21: _t->onActionEventActionEditorTriggered(); break;
        case 22: _t->updatePropertiesEditor((*reinterpret_cast< std::add_pointer_t<QWidget*>>(_a[1]))); break;
        case 23: _t->onPropertyItemChanged((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 24: _t->onInterfaceComboBoxChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 25: _t->onAddInterfaceAction(); break;
        case 26: _t->onDeleteInterfaceAction(); break;
        case 27: _t->onRenameInterfaceAction(); break;
        case 28: _t->onCopyInterfaceAction(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWidget* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *UILayoutWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UILayoutWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14UILayoutWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int UILayoutWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}
QT_WARNING_POP
