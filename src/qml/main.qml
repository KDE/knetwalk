/*
    Copyright 2013 Ashwin Rajeev <ashwin_rajeev@hotmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 1.1
import "logic.js" as Logic

Item {
    id: main
    property int cellCount
    property int selectedCell: 0
    property real border: 40
    property string state

    signal empty()
    signal clicked(int index, string direction)
    signal rotated(int index)

    CanvasItem {
        id: background
        spriteKey: "background"
        anchors.fill: parent
    }

    Grid {
        id: grid
        width:  (main.width > main.height)? main.height - border : main.width - border
        height: width; z: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        rows: cellCount; columns: cellCount;
    }

    CanvasItem {
        spriteKey: "overlay"
        x: grid.x
        y: grid.y
        width: (grid.width > 0)? grid.width : 0;
        height: (grid.height > 0)? grid.height : 0;
    }

    Rectangle {
        id: highlight
        color: "white"
        x: grid.x + ((grid.children.length > 0)? grid.children[selectedCell].x : 0)
        y: grid.y + ((grid.children.length > 0)? grid.children[selectedCell].y : 0)
        width: grid.width / cellCount
        height: grid.height / cellCount
        opacity: (main.state == "running")? 0.2 : 0
        Behavior on x {
            NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
        }
        Behavior on y {
            NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }
        }
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

    function setBoardSize(size) {
        Logic.reset();
        cellCount = size;
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
        if(state == "running") {
            selectedCell += (selectedCell < cellCount)? cellCount * (cellCount - 1): -cellCount;
        }
    }

    function kbGoDown() {
        if(state == "running") {
            selectedCell += (selectedCell < cellCount * (cellCount - 1))? cellCount: -cellCount * (cellCount - 1);
        }
    }

    function kbGoLeft() {
        if(state == "running") {
            selectedCell += (selectedCell % cellCount == 0)? cellCount - 1 : - 1;
        }
    }

    function kbGoRight() {
        if (state == "running") {
            selectedCell += (selectedCell % cellCount == cellCount - 1)? -cellCount + 1 : 1;
        }
    }

    function toggleLock() {
        if(state == "running") {
            Logic.cells[selectedCell].locked = !Logic.cells[selectedCell].locked;
        }
    }

    function setSprite(index, cable, type) {
        Logic.setSprite(index, cable, type);
    }

    function gameOver(msg) {
        state = msg;
    }
}
