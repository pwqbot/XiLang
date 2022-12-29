#include "lpf.h"
#include "ui_lpf.h"

#include <QFileDialog>
#include <algorithm>
#include <compiler/ast/ast_format.h>
#include <compiler/ast/type_assign.h>
#include <compiler/ast/type_format.h>
#include <compiler/generator/llvm.h>
#include <compiler/parser/program.h>
#include <qmessagebox.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <string.h>

lpf::lpf(QWidget *parent) : QMainWindow(parent), ui(new Ui::lpfClass)
{
    ui->setupUi(this);
}

lpf::~lpf() {}
void lpf::ShowftableToWindow(std::string)
{
    QString fileName = "ftable.txt";

    if (!fileName.isEmpty())
    {
        QFile *file = new QFile;
        file->setFileName(fileName);
        bool ok = file->open(QIODevice::ReadOnly);
        if (ok)
        {
            QTextStream in(file);
            ui->textEdit_4->setText(in.readAll());
            file->close();
            delete file;
        }
        else
        {
            QMessageBox::information(
                this, "������Ϣ", "���ļ�:" + file->errorString()
            );
            return;
        }
    }
}

void lpf::ShowAstToWindow(std::string /*unused*/ in)
{
    ui->textEdit_5->setText(in.c_str());
}

// ��ʾ������������
void lpf::ShowResultToWindow(std::string in)
{
    ui->textEdit_2->setText(in.c_str());
}
// ��ʾpcode����
void lpf::ShowPcodeToWindow(std::string in)
{
    ui->textEdit_3->setText(in.c_str());
}

auto exec(const char *cmd) -> std::string
{
    std::array<char, 128>                    buffer;
    std::string                              result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

void lpf::on_runButton_clicked()
{
    const std::string str = ui->textEdit->toPlainText().toStdString();

    spdlog::error(str);
    auto ast_result = xi::Xi_program(str);
    if (ast_result)
    {
        auto        ast_result_v = ast_result.value();
        std::string left_str     = std::string(ast_result_v.second);
        left_str.erase(
            left_str.begin(),
            std::find_if(
                left_str.begin(),
                left_str.end(),
                [](unsigned char ch)
                {
                    return std::isspace(ch) == 0;
                }
            )
        );
        if (left_str.empty())
        {
            spdlog::info("Parse successfully\n");
            spdlog::info("AST:\n {}\n", ast_result_v.first);

            ShowAstToWindow(fmt::format("{}", ast_result_v.first));

            auto ast_type = TypeAssign(ast_result_v.first);
            xi::ClearTypeAssignState();
            if (ast_type)
            {
                ShowAstToWindow(fmt::format("{}", ast_result_v.first));
                // write str to "temp.xi"
                std::ofstream out("temp.xi");
                out << str;
                std::flush(out);
                auto result = std::system("build/app/compiler/compiler temp.xi");
                if (result != 0) {
                    return;
                }
                // read from "a.ll"
                std::ifstream in("a.ll");
                auto          ll_str = std::string(
                    (std::istreambuf_iterator<char>(in)),
                    std::istreambuf_iterator<char>()
                );
                ShowPcodeToWindow(ll_str);
                    // use clang to link the object file
                auto final_result = exec("./a");
                ShowResultToWindow(final_result);
            }
            else
            {
                QMessageBox::information(
                    this, "Type Error!", ast_type.error().what().c_str()
                );
            }
        }
        else
        {
            auto pos = str.find(left_str);
            spdlog::error("Parse failed at {}:\n", pos);
            auto        l_pos   = pos - std::min(pos, 10ul);
            auto        r_pos   = std::min(pos + 10, str.size());
            auto        sub_str = str.substr(l_pos, r_pos - l_pos);
            std::string err_msg =
                fmt::format("pos : {}, content : {}", pos, sub_str);
            QMessageBox::information(this, "Parse fail!", err_msg.c_str());
        }
    }
    else
    {
        QMessageBox::information(this, "Parse Error", "???");
    }
}

void lpf::on_openButton_clicked()
{
    // ���ļ�ѡ��Ի���ѡ���ļ�����ȡ�ļ�·��
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("open code file"),
        "./test",
        tr("List files(*.xi);;All files (*.*)")
    );
    if (fileName.isEmpty())
    {
        ui->textEdit->setText("No file selected!");
    }
    else
    {
        QFile f(fileName);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ui->textEdit->setText("Fail����");
            return;
        }
        QTextStream txtInput(&f);
        txtInput.setCodec("utf-8");
        QString lineStr;
        while (!txtInput.atEnd())
        {
            lineStr = lineStr + txtInput.readLine() + "\n";
        }
        ui->textEdit->setText(lineStr); // ��ʾtxt�ļ�����
        ui->textEdit_2->clear();
        ui->textEdit_4->clear();
        ui->textEdit_5->clear();
        f.close();
    }
}
