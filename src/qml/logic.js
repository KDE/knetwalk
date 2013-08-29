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

function addCell(index, cable, type) {
    var cell = cellComponent.createObject(grid);
    cell.sprite = cable;
    cell.index = index;
    cell.type = type
    if(cable != "") {
        cableComponent.createObject(cell);
        createNode(type, cell)
    }
}

function createNode(type, cell) {
    if(type != "") {
        var node = canvasComponent.createObject(cell);
        node.spriteKey = function() { return cell.type };
        node.anchors.fill = cell;
        node.anchors.margins = function() { return cell.width / 10 };
    }
}

function cellAt(index) {
    return grid.children[index];
}

function reset() {
    selectedCell = 0
    state = "running"
    for(var i = grid.children.length; i > 0 ; i--) {
        cellAt(i-1).destroy();
    }
}

function setSprite(index, cable, type) {
    if(cellAt(index).angle == 0) {
        cellAt(index).sprite = cable;
        if (type != "none"){
            cellAt(index).type = type;
        }
    }
}

function rotate(direction) {
    if(cellAt(selectedCell).locked || cellAt(selectedCell).sprite == "") {
        empty();
    }
    else if(state == "running") {
        cellAt(selectedCell).angle += (direction == "clockwise")? 90 : -90;
        clicked(selectedCell, direction);
    }
}