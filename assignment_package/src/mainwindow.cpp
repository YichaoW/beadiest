#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    connect(ui->mygl,
        // Signal name
        SIGNAL(sig_created()),
        // Widget with the slot that receives the signal
        this,
        // Slot name
        SLOT(slot_addLabels())
    );

    connect(ui->vertsListWidget, SIGNAL(itemPressed(QListWidgetItem*)),
                ui->mygl, SLOT(slot_setSelectedVert(QListWidgetItem*)));

    connect(ui->halfEdgesListWidget, SIGNAL(itemPressed(QListWidgetItem*)),
                ui->mygl, SLOT(slot_setSelectedHE(QListWidgetItem*)));

    connect(ui->facesListWidget, SIGNAL(itemPressed(QListWidgetItem*)),
                ui->mygl, SLOT(slot_setSelectedFace(QListWidgetItem*)));

    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeBlue(double)));
    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeRed(double)));
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeGreen(double)));

    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeX(double)));

    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeY(double)));

    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
                ui->mygl, SLOT(slot_changeZ(double)));

    connect(ui->vertAddButton, SIGNAL(clicked(bool)),
                this, SLOT(slot_addLabel()));

    connect(ui->triangulateButton, SIGNAL(clicked(bool)),
                this, SLOT(slot_addTriangle()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_addTriangle() {
    ui->mygl->triangulate();
    for (const uPtr<Face> &f : ui->mygl->m_cube.faces) {
        ui->facesListWidget->addItem(f.get());
     }
    for (const uPtr<HalfEdge> &he : ui->mygl->m_cube.hes) {
        ui->halfEdgesListWidget->addItem(he.get());
     }
}

void MainWindow::slot_addLabel() {
    ui->mygl->addHalfEdge();
    for (const uPtr<Vertex> &v : ui->mygl->m_cube.vertices) {
        ui->vertsListWidget->addItem(v.get());
     }

    for (const uPtr<HalfEdge> &he : ui->mygl->m_cube.hes) {
        ui->halfEdgesListWidget->addItem(he.get());
     }
}

void MainWindow::slot_addLabels() {
    for (const uPtr<Face> &f : ui->mygl->m_cube.faces) {
        ui->facesListWidget->addItem(f.get());
     }

    for (const uPtr<Vertex> &v : ui->mygl->m_cube.vertices) {
        ui->vertsListWidget->addItem(v.get());
     }

    for (const uPtr<HalfEdge> &he : ui->mygl->m_cube.hes) {
        ui->halfEdgesListWidget->addItem(he.get());
     }
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}
