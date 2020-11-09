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

import QtQuick 2.3

Item{
    property string sprite
    property string type
    property int index
    property int angle
    property bool locked: false

    width: grid.width / grid.columns;
    height: width;

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: (locked && main.state === "running")? 0.5 : 0
    }

    MouseArea {
        anchors.fill: parent
        enabled: main.state === "running"
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        hoverEnabled: true
        onClicked: {
            main.selected = index
            switch (mouse.button) {
                case Qt.LeftButton:
                    if (main.reverseButtons) {
                        main.rotateClockwise();
                    }
                    else {
                        main.rotateCounterclockwise();
                    }
                    break;
                case Qt.RightButton:
                    if (main.reverseButtons) {
                        main.rotateCounterclockwise();
                    }
                    else {
                        main.rotateClockwise();
                    }
                    break;
                case Qt.MiddleButton:
                    locked = !locked;
                    break;
            }
        }
        onEntered: main.selected = index
        onPositionChanged: main.selected = index
    }
}
