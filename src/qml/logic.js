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

var border = 40
var cellComponent = Qt.createComponent("Cell.qml");
var cableComponent = Qt.createComponent("Cable.qml");
var canvasComponent = Qt.createComponent("CanvasItem.qml");
var cells = []
var nodes = []

function overlaySize() {
    var size = (width > height)? height - border : width - border;
    return (size > 0)? size : 0;
}

function gridWidth() {
    var widthRatio = (width - border) / grid.columns;
    var heightRatio = (height - border) / grid.rows;
    var gWidth = grid.columns * Math.min(widthRatio, heightRatio);
    return (gWidth > 0)? gWidth : 0;
}

function gridHeight() {
    var widthRatio = (width - border) / grid.columns;
    var heightRatio = (height - border) / grid.rows;
    var gHeight = grid.rows * Math.min(widthRatio, heightRatio);
    return (gHeight > 0)? gHeight : 0;
}

function addCell(cable, type) {
    var cell = cellComponent.createObject(grid);
    cell.sprite = cable;
    cell.index = cells.length;
    cell.type = type
    if(cable !== "") {
        cableComponent.createObject(cell);
        createNode(type, cell)
    }
    cells.push(cell);
}

function createNode(type, cell) {
    if(type !== "none") {
        var node = canvasComponent.createObject(cell);
        node.spriteKey = cell.type;
        node.anchors.fill = cell;
        node.anchors.margins = cell.width / 10;
        nodes[cell]=node;
    }
}

function reset(width, height) {
    selected = 0
    state = "running"
    while(cells.length > 0) {
        var cell = cells.pop();
        cell.visible = false;
        cell.destroy();
    }
    rows = height;
    columns = width;
}

function setSprite(index, cable, type) {
    cells[index].angle = 0;
    cells[index].sprite = cable;
    if (type !== "none"){
        cells[index].type = type;
        nodes[cells[index]].spriteKey = type;
    }
}

function rotate(direction) {
    if(cells[selected].locked || cells[selected].sprite === "") {
        clicked(-1); //invalid click
    }
    else if(state === "running") {
        cells[selected].angle += (direction === "clockwise")? 90 : -90;
        clicked(selected);
    }
}

function unlockAll() {
    for(var i = 0; i < cells.length; i++) {
        cells[i].locked = false;
    }
}
