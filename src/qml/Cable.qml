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

CanvasItem {
    spriteKey: "cable" + parent.sprite
    anchors.fill: parent
    rotation: parent.angle
    opacity: (main.state === "paused")? 0 : 1
    Behavior on rotation {
        id: animate
        NumberAnimation {
            duration: main.rotateDuration
            easing.type: Easing.OutQuad
            onRunningChanged: {
                if(!running && parent.angle != 0) {
                    animate.enabled = false
                    main.rotated(parent.index, parent.angle % 360);
                    animate.enabled = true
                }
            }
        }
    }
}
