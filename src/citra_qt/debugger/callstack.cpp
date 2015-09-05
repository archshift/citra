// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <QStandardItemModel>

#include "citra_qt/debugger/callstack.h"

#include "common/common_types.h"

#include "core/core.h"
#include "core/arm/arm_interface.h"

CallstackWidget::CallstackWidget(QWidget* parent): QDockWidget(parent)
{
    ui.setupUi(this);

    callstack_model = new QStandardItemModel(this);
    callstack_model->setColumnCount(1);
    callstack_model->setHeaderData(0, Qt::Horizontal, "Caller address");
    ui.treeView->setModel(callstack_model);
}

void CallstackWidget::OnDebugModeEntered()
{
    Clear();

    std::vector<u32> stack_trace = Core::g_app_core->GetStackTrace();
    for (auto entry : stack_trace) {
        callstack_model->setItem(callstack_model->rowCount(), new QStandardItem(QString("0x%1").arg(entry, 8, 16, QLatin1Char('0'))));
    }
}

void CallstackWidget::OnDebugModeLeft()
{

}

void CallstackWidget::Clear()
{
    callstack_model->removeRows(0, callstack_model->rowCount());
}
