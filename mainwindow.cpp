/*  mainwindow.cpp
 * Notes:
 *Using struct to store information about each disk.
 * Using vector to store which disks are in each peg.
 * There are methods for each button and timer, button enable,
 * disk color change, disk moving, and checking game is solved.
 *
 * implementation:
 * You can start game with input the disks number from 1 to 8.
 * If you put the wrong input, you cannot start the game and you will see the error message box.
 * Then you need to put another number of disks.
 * If you put the right disks number, press the start button.
 * Only possible direction button would be activated.
 * If you want to pause the game, or to check the game time, press the stop button.
 * If you want to close the window, press close button.
 * If you want to start the game from beginning, press reset button.
 * The required movements will show the minimum number of movements to solve the game.
 * The movements will show you how many times you move the disks.
 * Game history will show you what kind of moving direction you've already done.
 * When game is complete, you will see the messagebox and timer will stop automatically.
 * From completed game, you only can close the window or reset the game.
 * */

#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <math.h>
#include <QString>
#include <QMessageBox>
#include <QVector>
#include <QGraphicsRectItem>
#include <QList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // For timer function
    timer = new QTimer(this);
    connect (timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer->start(10);

    // We need a graphics scene in which to draw a circle
    scene = new QGraphicsScene(this);
    // The graphicsView object is placed on the window
    // at the following coordinates:
    int left_margin = 10; // x coordinate
    int top_margin = 270; // y coordinate
    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(left_margin, top_margin,
                                   BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1,
    // because the circle is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTimer()
{
    minutes = (totalTime / 1000) / 60;
    seconds = (totalTime / 1000) % 60;
    ui->MinLcdNumber->display(minutes);
    ui->SecLcdNumber->display(seconds);
}

void MainWindow::on_StartButton_clicked()
{
    // Get disks number and calculate minimum movements
    if (!is_running)
    {
        disksNumber = ui->DiskNumber->text().toInt();
        if (disksNumber > 0 and disksNumber < 9)
        {
            game_start++;
            // Cannot change the disks number while playing
            ui->DiskNumber->setReadOnly(true);
            // Calculate minimum movements
            minimumNumber = pow(2,disksNumber) - 1;
            // Show how many movements is done
            ui->MinimumMovement->setText(QString::number(minimumNumber));
            ui->MovementNumber->setText(QString::number(movements));
            // Timer start
            is_running = true;
            startTime = QDateTime::currentDateTime();
            int tempTime = startTime.msecsTo(QDateTime::currentDateTime());
            totalTime += tempTime;
            if (game_start == 1)
            {
                // Disks appear at source peg (A) with color of peg (red)
                // Defining the color and outline of the rect
                QBrush redBrush(Qt::red);
                QPen blackPen(Qt::black);
                blackPen.setWidth(2);
                //draw disks
                Disk newDisk;
                for (int i = 0; i < disksNumber; i++)
                {
                    newDisk.floor = i;
                    // multiply 2 because left and right get shrink
                    int newWidth = WIDTH - (DELTA_W * 2 * i);
                    int newX = GROUND_X + DELTA_W * i;
                    int newY = GROUND_Y - 15 * i;
                    newDisk.disk = scene->addRect(newX, newY, newWidth, HEIGHT, blackPen, redBrush);
                    vectorA.push_back(newDisk);
                }
            }
            // button enable depends on where the disks are
            buttonEnable(vectorA, vectorB, vectorC);
        }
        else
        {
            QMessageBox::information(this, tr("ERROR"), tr("Invalid number of disks. Put int from 1 to 8"));
        }
    }
}


void MainWindow::on_StopButton_clicked()
{
    // Timer stop
    if (is_running)
    {
        is_running = false;
        int tempTime = startTime.msecsTo(QDateTime::currentDateTime());
        totalTime += tempTime;
        // push button disabled
        ui->ABbutton->setEnabled(false);
        ui->ACbutton->setEnabled(false);
        ui->BAbutton->setEnabled(false);
        ui->BCbutton->setEnabled(false);
        ui->CAbutton->setEnabled(false);
        ui->CBbutton->setEnabled(false);
    }
}

void MainWindow::on_ResetButton_clicked()
{
    is_running = false;
    game_start = 0;
    // Can change the disk number
    ui->DiskNumber->setReadOnly(false);
    // Reset timer
    totalTime = 0;
    minutes = 0;
    seconds = 0;
    ui->MinLcdNumber->display(minutes);
    ui->SecLcdNumber->display(seconds);
    // Reset disks number
    ui->DiskNumber->clear();
    // Reset game history
    ui->GameHistory->clear();
    minimumNumber = 0;
    movements = 0;
    ui->MinimumMovement->clear();
    ui->MovementNumber->clear();
    // Push button disabled
    ui->ABbutton->setEnabled(false);
    ui->ACbutton->setEnabled(false);
    ui->BAbutton->setEnabled(false);
    ui->BCbutton->setEnabled(false);
    ui->CAbutton->setEnabled(false);
    ui->CBbutton->setEnabled(false);
    // Delete disks
    scene->clear();
    // clear the vectors for pegs
    vectorA.clear();
    vectorB.clear();
    vectorC.clear();
    // After game is complete and reset the game,
    // need to activate start and stop button
    ui->StartButton->setEnabled(true);
    ui->StopButton->setEnabled(true);
}

void MainWindow::on_ABbutton_clicked()
{
    ui->GameHistory->append("A->B");
    movements++;
    ui->MovementNumber->setText(QString::number(movements));
    // Move disks
    is_right = true;
    stepNumber = 1;
    // Disk color changes according to the peg color
    Disk upperDisk = vectorA.at(vectorA.size() - 1);
    disksColorChange('B', upperDisk);
    // move the most upper disk
    moveDisk(vectorA, vectorB);
}

void MainWindow::on_ACbutton_clicked()
{
    ui->GameHistory->append("A->C");
    movements++;
    ui->MovementNumber->setText(QString::number(movements));
    // Move disks
    is_right = true;
    stepNumber = 2;
    // Disk color changes according to the peg color
    Disk upperDisk = vectorA.at(vectorA.size() - 1);
    disksColorChange('C', upperDisk);
    // move the most upper disk
    moveDisk(vectorA, vectorC);
}

void MainWindow::on_BAbutton_clicked()
{
     ui->GameHistory->append("B->A");
     movements++;
     ui->MovementNumber->setText(QString::number(movements));
     // Move disks
     is_right = false;
      stepNumber = 1;
      // Disk color changes according to the peg color
      Disk upperDisk = vectorB.at(vectorB.size() - 1);
      disksColorChange('A', upperDisk);
      // move the most upper disk
      moveDisk(vectorB, vectorA);
}

void MainWindow::on_BCbutton_clicked()
{
    ui->GameHistory->append("B->C");
    movements++;
    ui->MovementNumber->setText(QString::number(movements));
    // Move disks
    is_right = true;
    stepNumber = 1;
    // Disk color changes according to the peg color
    Disk upperDisk = vectorB.at(vectorB.size() - 1);
    disksColorChange('C', upperDisk);
    // move the most upper disk
    moveDisk(vectorB, vectorC);
}

void MainWindow::on_CAbutton_clicked()
{
    ui->GameHistory->append("C->A");
    movements++;
    ui->MovementNumber->setText(QString::number(movements));
    // Move disks
    is_right = false;
    stepNumber = 2;
    // Disk color changes according to the peg color
    Disk upperDisk = vectorC.at(vectorC.size() - 1);
    disksColorChange('A', upperDisk);
    // move the most upper disk
    moveDisk(vectorC, vectorA);
}

void MainWindow::on_CBbutton_clicked()
{
    ui->GameHistory->append("C->B");
    movements++;
    ui->MovementNumber->setText(QString::number(movements));
    // Move disks
    is_right = false;
    stepNumber = 1;
    // Disk color changes according to the peg color
    Disk upperDisk = vectorC.at(vectorC.size() - 1);
    disksColorChange('B', upperDisk);
    // move the most upper disk
    moveDisk(vectorC, vectorB);
}

// Check where is the disks and enable the buttons
void MainWindow::buttonEnable(QVector <Disk> A, QVector <Disk> B, QVector <Disk> C)
{
    // Get the max floor of each peg
    int maxA = -1;
    int maxB = -1;
    int maxC = -1;
    // Get the max floor of each peg
    if (A.size() > 0)
    {
        maxA = A.at(A.size() - 1).floor;
    }
    if (B.size() > 0)
    {
        maxB = B.at(B.size() - 1).floor;
    }
    if (C.size() > 0)
    {
        maxC = C.at(C.size() - 1).floor;
    }
    qDebug() <<"MAX" << maxA << ", " << maxB <<", "<< maxC;
   // Check disk may not be placed on top of smaller disk.
    if (vectorA.size() > 0 and maxA > maxB)
    {
        ui->ABbutton->setEnabled(true);
    }
    else
    {
        ui->ABbutton->setEnabled(false);
    }
    if (vectorA.size() > 0 and maxA > maxC)
    {
        ui->ACbutton->setEnabled(true);
    }
    else
    {
        ui->ACbutton->setEnabled(false);
    }
    if (vectorB.size() > 0 and maxB > maxA)
    {
        ui->BAbutton->setEnabled(true);
    }
    else
    {
        ui->BAbutton->setEnabled(false);
    }
    if (vectorB.size() > 0 and maxB > maxC)
    {
        ui->BCbutton->setEnabled(true);
    }
    else
    {
        ui->BCbutton->setEnabled(false);
    }
    if (vectorC.size() > 0 and maxC > maxA)
    {
        ui->CAbutton->setEnabled(true);
    }
    else
    {
        ui->CAbutton->setEnabled(false);
    }
    if (vectorC.size() > 0 and maxC >maxB)
    {
        ui->CBbutton->setEnabled(true);
    }
    else
    {
        ui->CBbutton->setEnabled(false);
    }
}

// Change the disks color
void MainWindow::disksColorChange(char to, Disk upperDisk)
{
    QBrush redBrush(Qt::red);
    QBrush greenBrush(Qt::green);
    QBrush blueBrush(Qt::blue);
    switch (to) {
    case 'A':
        upperDisk.disk->setBrush(redBrush);
        break;
    case 'B':
        upperDisk.disk->setBrush(greenBrush);
        break;
    case 'C':
        upperDisk.disk->setBrush(blueBrush);
        break;
    default:
        break;
    }
}

// Move disks
void MainWindow::moveDisk(QVector <Disk> &from, QVector <Disk> &to)
{
    qreal deltaX, deltaY;
    // Get the number in source peg(from) and target peg(to)
    int sourceHeight;
    int targetHeight;
    // Calculate deltaY depends on how many disks in pegs
   sourceHeight = from.size();
   targetHeight = to.size();
   // Check current axis
    qreal current_x = from.at(sourceHeight - 1).disk->x();
    qreal current_y = from.at(sourceHeight - 1).disk->y();    
    // move to right
    if (is_right)
    {
        deltaX = static_cast<qreal>(STEP) * stepNumber;
    }
    // move to left
    else
    {
        deltaX = static_cast<qreal>(-STEP) * stepNumber;
    }
    current_x += deltaX;
    deltaY = static_cast<qreal>(DOWN_STEP * (sourceHeight - targetHeight - 1));
    current_y += deltaY;
    from.at(sourceHeight - 1).disk->moveBy(deltaX, deltaY);
    to.push_back(from.at(sourceHeight - 1));
    from.pop_back();
    // Test print
    qDebug() <<"A "<< vectorA.size() << ", B " << vectorB.size()<<", C" << vectorC.size();
    qDebug() << isGameSolved();
    // button enable depends on where the disks are
    buttonEnable(vectorA, vectorB, vectorC);
    if (isGameSolved())
    {
        QMessageBox::information(this, tr("NOTICE"), tr("Game is completed. Congrats! Check the game playing time."));
        // Stop button activated
        ui->StopButton->clicked();
        // Cannot start or stop the game again after the game is completed
        ui->StopButton->setEnabled(false);
        ui->StartButton->setEnabled(false);
    }
}

// Check the game is complete
bool MainWindow::isGameSolved()
{
    if (vectorC.size() == disksNumber)
    {
        QVector <int> inOrder;
        for (int i = 0; i < disksNumber; i++)
        {
            inOrder.push_back(i);
        }
        QVector <int> floors;
        for (auto item: vectorC)
        {
            floors.push_back(item.floor);
        }
        if (floors == inOrder)
        {
            return true;
        }
    }
    return false;
}
