/*
 * Copyright (C) 2019-2020 Ashar Khan <ashar786khan@gmail.com>
 *
 * This file is part of CPEditor.
 *
 * CPEditor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * I will not be responsible if CPEditor behaves in unexpected way and
 * causes your ratings to go down and or loose any important contest.
 *
 * Believe Software is "Software" and it isn't immune to bugs.
 *
 */

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "Core/Checker.hpp"
#include "Core/Compiler.hpp"
#include "Core/Formatter.hpp"
#include "Core/Runner.hpp"
#include "Core/SettingsManager.hpp"
#include "Extensions/CFTools.hpp"
#include "Extensions/CompanionServer.hpp"
#include "Telemetry/UpdateNotifier.hpp"
#include "Widgets/TestCases.hpp"
#include <QCodeEditor>
#include <QFile>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QShortcut>
#include <QSplitter>
#include <QTemporaryDir>
#include <generated/version.hpp>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    struct EditorStatus
    {
        bool isLanguageSet, isHideAC;
        QString filePath, savedText, problemURL, editorText, language;
        int editorCursor, editorAnchor, horizontalScrollBarValue, verticalScrollbarValue, untitledIndex, checkerIndex;
        QStringList input, expected, customCheckers;

        EditorStatus(){};

        explicit EditorStatus(const QMap<QString, QVariant> &status);

        QMap<QString, QVariant> toMap() const;
    };

    explicit MainWindow(const QString &fileOpen, int index = 0, QWidget *parent = nullptr);
    ~MainWindow() override;

    int getUntitledIndex() const;
    QString getFileName() const;
    QString getFilePath() const;
    QString getProblemURL() const;
    QString getCompleteTitle() const;
    QString getTabTitle(bool complete, bool star, int removeLength = 0);
    QCodeEditor *getEditor() const;
    bool isUntitled() const;

    void setProblemURL(const QString &url);
    void setUntitledIndex(int index);

    EditorStatus toStatus(bool simple) const;
    void loadStatus(const EditorStatus &status, bool simple);

    void save(bool force, const QString &head, bool safe = true);
    void saveAs();

    bool isTextChanged();
    bool closeConfirm();

    void killProcesses();
    void detachedExecution();
    void compileOnly();
    void runOnly();
    void compileAndRun();
    void formatSource();

    void applyCompanion(const Network::CompanionData &data);

    void setLanguage(const QString &lang);
    QString getLanguage();
    void applySettings(bool);

    MessageLogger *getLogger();
    QSplitter *getSplitter();
    QSplitter *getRightSplitter();

    void insertText(const QString &text);

    void setViewMode(Settings::ViewMode mode);

  private slots:
    void on_compile_clicked();
    void on_runOnly_clicked();
    void on_run_clicked();

    void onCompilationStarted();
    void onCompilationFinished(const QString &warning);
    void onCompilationErrorOccured(const QString &error);
    void onCompilationKilled();

    void onRunStarted(int index);
    void onRunFinished(int index, const QString &out, const QString &err, int exitCode, int timeUsed);
    void onRunErrorOccured(int index, const QString &error);
    void onRunTimeout(int index);
    void onRunKilled(int index);

    void on_changeLanguageButton_clicked();

    void onFileWatcherChanged(const QString &);

    void onTextChanged();

    void updateCursorInfo();

    void updateChecker();

  signals:
    void editorFileChanged();
    void editorTextChanged(MainWindow *window);
    void confirmTriggered(MainWindow *widget);

  private:
    enum SaveMode
    {
        IgnoreUntitled, // save only when filePath is not empty
        SaveUntitled,   // save to filePath if it's not empty, otherwise ask for new path
        SaveAs,         // ask for new path no matter filePath is empty or not
    };
    enum AfterCompile
    {
        Nothing,
        Run,
        RunDetached
    };
    enum Verdict
    {
        ACCEPTED,
        WRONG_ANSWER,
        UNKNOWN
    };

    Ui::MainWindow *ui;
    QCodeEditor *editor;
    QString language;
    bool isLanguageSet = false;

    Core::Formatter *formatter = nullptr;
    Core::Compiler *compiler = nullptr;
    QVector<Core::Runner *> runner;
    Core::Checker *checker = nullptr;
    Core::Runner *detachedRunner = nullptr;
    QTemporaryDir *tmpDir = nullptr;
    AfterCompile afterCompile = Nothing;

    MessageLogger log;

    int untitledIndex;
    QString problemURL;
    QString filePath;
    QString savedText;
    QString cftoolPath;
    QFileSystemWatcher *fileWatcher;
    bool reloading = false;

    QPushButton *submitToCodeforces = nullptr;
    Network::CFTools *cftools = nullptr;

    TestCases *testcases = nullptr;

    void setTestCases();
    void setEditor();
    void setupCore();
    void compile();
    void run();
    void loadTests();
    void saveTests(bool safe);
    void setCFToolsUI();
    void setText(const QString &text, bool keep = false);
    void updateWatcher();
    void loadFile(const QString &loadPath);
    bool saveFile(SaveMode, const QString &head, bool safe);
    bool saveTemp(const QString &head);
    QString tmpPath();
    void performCoreDiagonistics();
    QString getRunnerHead(int index);
};
#endif // MAINWINDOW_HPP
