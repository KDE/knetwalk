/*
    SPDX-FileCopyrightText: 2013 Ashwin Rajeev <ashwin_rajeev@hotmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.3
import "logic.js" as Logic

Item {
    id: main
    property int selected: 0
    property int rotateDuration: 300
    property bool reverseButtons: false
    property string state
    property alias rows: grid.rows
    property alias columns: grid.columns

    signal clicked(int index)
    signal rotated(int index, int angle)

    CanvasItem {
        id: background
        spriteKey: "background"
        anchors.fill: parent
    }

    Grid {
        id: grid
        width:  Logic.gridWidth()
        height: Logic.gridHeight()
        z: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    CanvasItem {
        spriteKey: "overlay"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: Logic.overlaySize()
        height: width
    }

    Rectangle {
        id: highlight
        color: "white"
        x: grid.x + ((Logic.cells.length > selected)? Logic.cells[selected].x : 0)
        y: grid.y + ((Logic.cells.length > selected)? Logic.cells[selected].y : 0)
        width: grid.width / grid.columns
        height: width
        opacity: (main.state === "running")? 0.2 : 0
    }

    Rectangle {
        id: pauseMessage
        width: parent.width / 2
        height: parent.height / 4
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        color: "black"; radius: 10; z: 1
        opacity: (main.state === "paused") ? 0.8 : 0

        Text {
            color: "white"
            text: i18n("Paused")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: parent.width / 8
        }
    }

    function addCell(cable, type) {
        Logic.addCell(cable, type);
    }

    function setBoardSize(width, height) {
        Logic.reset(width, height);
    }

    function rotateClockwise() {
        Logic.rotate("clockwise");
    }

    function rotateCounterclockwise() {
        Logic.rotate("counterclockwise");
    }

    function pause(paused) {
        state = (paused)? "paused" : "running";
    }

    function kbGoUp() {
        if(state === "running") {
            selected += (selected < columns)? columns * (rows - 1) : -columns;
        }
    }

    function kbGoDown() {
        if(state === "running") {
            selected += (selected < columns * (rows - 1))? columns : -columns * (rows - 1);
        }
    }

    function kbGoLeft() {
        if(state === "running") {
            selected += (selected % columns == 0)? columns - 1 : - 1;
        }
    }

    function kbGoRight() {
        if (state === "running") {
            selected += (selected % columns == columns - 1)? -columns + 1 : 1;
        }
    }

    function toggleLock() {
        if(state === "running") {
            Logic.cells[selected].locked = !Logic.cells[selected].locked;
        }
    }

    function lock(index) {
        Logic.cells[index].locked = true;
    }

    function unlockAll() {
        Logic.unlockAll();
    }

    function setSprite(index, cable, type) {
        Logic.setSprite(index, cable, type);
    }

    function gameOver(msg) {
        state = msg;
    }
}
