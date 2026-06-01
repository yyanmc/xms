#include "task_list_gui.h"
#include "taskitemgui.h"
#include "xfile_manager.h"
#include <QDebug>
#include <map>
#include <string>
#include <sstream>
using namespace std;
// key = (index << 1) | (is_sec ? 1 : 0) 区分上传和下载任务
static map<long long, TaskItemGUI*> task_items;
void TaskListGUI::RefreshUploadTask(std::list<xdisk::XFileTask> file_list)
{
    upload_list = file_list;
    // 过滤出未完成的任务用于显示和计数
    list<xdisk::XFileTask> active;
    for (auto& t : file_list)
        if (!t.is_complete()) active.push_back(t);

    if (active.empty())
    {
        ui.uplabel->setText("");
        if (ui.upButton->isChecked())
        {
            task_items.clear();
            while (ui.taskableWidget->rowCount() > 0)
                ui.taskableWidget->removeRow(0);
        }
        return;
    }
    stringstream ss;
    ss << "(" << active.size() << ")";
    ui.uplabel->setText(ss.str().c_str());
    if (!ui.upButton->isChecked())
        return;
    RefreshTask(active);
}

void TaskListGUI::RefreshDownloadTask(std::list<xdisk::XFileTask> file_list)
{
    download_list = file_list;
    // 过滤出未完成的任务用于显示和计数
    list<xdisk::XFileTask> active;
    for (auto& t : file_list)
        if (!t.is_complete()) active.push_back(t);

    if (active.empty())
    {
        ui.downlabel->setText("");
        if (ui.downButton->isChecked())
        {
            task_items.clear();
            while (ui.taskableWidget->rowCount() > 0)
                ui.taskableWidget->removeRow(0);
        }
        return;
    }
    stringstream ss;
    ss << "(" << active.size() << ")";
    ui.downlabel->setText(ss.str().c_str());
    if (!ui.downButton->isChecked())
        return;
    RefreshTask(active);
}

void TaskListGUI::RefreshTask(std::list<xdisk::XFileTask> file_list)
{
    //只修改 不清理
    auto tab = ui.taskableWidget;
    for(auto task: file_list)
    {
        long long key = ((long long)task.index() << 1) | (task.is_sec() ? 1 : 0);
        if (task_items.find(key) == task_items.end())
        {
            tab->insertRow(0);
            auto item = new TaskItemGUI();
            item->SetTask(task);
            tab->setCellWidget(0, 0, item);

            tab->setRowHeight(0,51);
            task_items[key] = item;
        }
        else
        {
            task_items[key]->SetTask(task);
        }
    }
}


TaskListGUI::TaskListGUI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    auto tab = ui.taskableWidget;
    while (tab->rowCount() > 0)
    {
        tab->removeRow(0);
    }
    tab->setIconSize(QSize(30, 30));
    tab->setColumnWidth(0, 100);
    tab->setColumnWidth(1, 500);

    tab->setSelectionBehavior(QAbstractItemView::SelectRows);
    tab->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.uplabel->setText("");
    ui.downlabel->setText("");
    ui.oklabel->setText("");
    connect(XFileManager::Instance(), SIGNAL(RefreshCompletedTaskCount(int)), this, SLOT(UpdateCompletedLabel(int)));
}

void TaskListGUI::OkTask()
{
    auto tasks = XFileManager::Instance()->GetCompletedTasks();
    // 更新计数
    if (tasks.empty())
        ui.oklabel->setText("");
    else
    {
        stringstream ss;
        ss << "(" << tasks.size() << ")";
        ui.oklabel->setText(ss.str().c_str());
    }

    auto tab = ui.taskableWidget;
    task_items.clear();
    while (tab->rowCount() > 0)
        tab->removeRow(0);
    RefreshTask(tasks);
}
void TaskListGUI::UpTask()
{
    auto tab = ui.taskableWidget;
    task_items.clear();
    while (tab->rowCount() > 0)
        tab->removeRow(0);
    // 显示未完成的任务
    list<xdisk::XFileTask> active;
    for (auto& t : upload_list)
        if (!t.is_complete()) {
            active.push_back(t);
            active.back().set_is_sec(true);  // 标记为上传
        }
    RefreshTask(active);
}

void TaskListGUI::DownTask()
{
    task_items.clear();
    auto tab = ui.taskableWidget;
    while (tab->rowCount() > 0)
        tab->removeRow(0);
    // 显示未完成的任务
    list<xdisk::XFileTask> active;
    for (auto& t : download_list)
        if (!t.is_complete()) active.push_back(t);
    RefreshTask(active);
}

void TaskListGUI::RefreshCurrentTab()
{
    if (ui.okButton->isChecked())
        OkTask();
    else if (ui.downButton->isChecked())
        DownTask();
    else if (ui.upButton->isChecked())
        UpTask();
}

void TaskListGUI::UpdateCompletedLabel(int count)
{
    if (count == 0)
        ui.oklabel->setText("");
    else
    {
        stringstream ss;
        ss << "(" << count << ")";
        ui.oklabel->setText(ss.str().c_str());
    }
}

void TaskListGUI::Select(QModelIndex index)
{

}

TaskListGUI::~TaskListGUI()
{
}

void TaskListGUI::Hide()
{
    this->hide();
}
void TaskListGUI::Show()
{
    this->show();
    QWidget *p = (QWidget *)this->parent();
    int w = p->width();
    auto tab_pos = ui.taskableWidget->pos();
    auto size = ui.taskableWidget->size();

    size.setHeight(p->height() - pos().y());
    size.setWidth(w- tab_pos.x());
    ui.taskableWidget->resize(size);
}
