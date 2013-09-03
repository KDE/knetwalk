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

var cellComponent = Qt.createComponent("Cell.qml");
var cableComponent = Qt.createComponent("Cable.qml");
var canvasComponent = Qt.createComponent("CanvasItem.qml");
var cells = []

function addCell(cable, type) {
    var cell = cellComponent.createObject(grid);
    cell.sprite = cable;
    cell.index = cells.length;
    cell.type = type
    if(cable != "") {
        cableComponent.createObject(cell);
        createNode(type, cell)
    }
    cells.push(cell);
}

function createNode(type, cell) {
    if(type != "none") {
        var node = canvasComponent.createObject(cell);
        node.spriteKey = function() { return cell.type };
        node.anchors.fill = cell;
        node.anchors.margins = function() { return cell.width / 10 };
    }
}

function reset() {
    selectedCell = 0
    state = "running"
    while(cells.length > 0) {
        var cell = cells.pop();
        cell.visible = false;
        cell.destroy();
    }
}

function setSprite(index, cable, type) {
    if(cells[index].angle == 0) {
        cells[index].sprite = cable;
        if (type != "none"){
            cells[index].type = type;
        }
    }
}

function rotate(direction) {
    if(cells[selectedCell].locked || cells[selectedCell].sprite == "") {
        empty();
    }
    else if(state == "running") {
        cells[selectedCell].angle += (direction == "clockwise")? 90 : -90;
        clicked(selectedCell, direction);
    }
}