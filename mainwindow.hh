/*  mainwindow.cpp
 *
 * Notes:
 * Using struct to store information about each disk.
 * Using vector to store which disks are in each peg.
 * There are methods for each button and timer, button enable,
 * disk color change, disk moving, and checking game is solved.
 *
 * */
#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTimer* timer;

    // struct for disk object and which floor it initialized
    struct Disk
    {
        int floor;
        QGraphicsRectItem* disk;
    };

private slots:
    // timer updated
    void updateTimer();

    void on_StartButton_clicked();
    void on_StopButton_clicked();
    void on_ResetButton_clicked();
    void on_ABbutton_clicked();
    void on_ACbutton_clicked();
    void on_BAbutton_clicked();
    void on_BCbutton_clicked();
    void on_CAbutton_clicked();
    void on_CBbutton_clicked();
    // Check where is the disks and enable the buttons
    void buttonEnable(QVector <Disk> A, QVector <Disk> B, QVector <Disk> C);
    // Change the disks color
    void disksColorChange(char to, Disk upperDisk);
    // Move disks
    void moveDisk(QVector <Disk> &from, QVector <Disk> &to);
    // Check the game is complete
    bool isGameSolved();

private:
    Ui::MainWindow *ui;
    QGraphicsScene* scene;          // a surface for
    QGraphicsRectItem* disk;   // drawing a rect
    const int WIDTH = 80; // width of rect
    const int HEIGHT = 15;
    const int STEP = 120;            // how much to move at a time
    const int DOWN_STEP = 15;
    int stepNumber;
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 260;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 680;
    bool is_right;
    const int GROUND_X = 107;
    const int GROUND_Y = 140;
    const int DELTA_W = 5;

    // for timer
    int minutes = 0;
    int seconds = 0;
    int totalTime = 0;
    bool is_running = false;
    int TempTime;
    QDateTime startTime;
    //for movement count
    int minimumNumber = 0;
    int movements = 0;
    int game_start = 0;
    // Number of disks by user input
    int disksNumber;
    // vector for each peg
    QVector <Disk> vectorA;
    QVector <Disk> vectorB;
    QVector <Disk> vectorC;

};

#endif // MAINWINDOW_HH
