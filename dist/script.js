(function(){function r(e,n,t){function o(i,f){if(!n[i]){if(!e[i]){var c="function"==typeof require&&require;if(!f&&c)return c(i,!0);if(u)return u(i,!0);var a=new Error("Cannot find module '"+i+"'");throw a.code="MODULE_NOT_FOUND",a}var p=n[i]={exports:{}};e[i][0].call(p.exports,function(r){var n=e[i][1][r];return o(n||r)},p,p.exports,r,e,n,t)}return n[i].exports}for(var u="function"==typeof require&&require,i=0;i<t.length;i++)o(t[i]);return o}return r})()({1:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.App = void 0;
const ListView_1 = require("./ListView");
const MatrixView_1 = require("./MatrixView");
class App {
    constructor(params) {
        this.snakeColor = "";
        this.generations = 0;
        this.snakeInfo = "";
        this.params = params;
        this.screenContext = params.screen.getContext("2d");
        params.screen.width = params.blockSize * params.boardSize;
        params.screen.height = params.screen.width;
    }
    async load() {
        const module = await fetch("backend.wasm");
        const buffer = await module.arrayBuffer();
        const instance = await WebAssembly.instantiate(buffer, {
            env: {
                randomFloat: Math.random,
                randomInt: this.randomInt
            }
        });
        this.backend = instance.instance.exports;
    }
    makeDecision(snake, brain) {
        const distanceMatrix = this.backend.snakeCalcDistanceMatrix(snake);
        const y = this.backend.networkFit(brain, distanceMatrix);
        let direction = 0;
        let value = 0;
        new MatrixView_1.MatrixView(this.backend.memory.buffer, y).forEach((v, i) => {
            if (v > value) {
                value = v;
                direction = i;
            }
        });
        let str = "In:\n";
        new MatrixView_1.MatrixView(this.backend.memory.buffer, distanceMatrix).forEach(v => {
            str += `${Math.round(100 * v) / 100}\n`;
        });
        this.snakeInfo = str + "\n\nOut:\n" + ["up", "right", "down", "left"][direction];
        this.updateInfo();
        this.backend.matrixDelete(distanceMatrix);
        this.backend.matrixDelete(y);
        return direction;
    }
    run() {
        const evolution = this.backend.jsEvolutionCreate();
        this.backend.evolutionInit(evolution);
        let best = this.backend.evolutionGetBestNetwork(evolution);
        setInterval(() => {
            this.backend.evolutionNextGeneration(evolution);
            best = this.backend.evolutionGetBestNetwork(evolution);
            this.generations += 1;
            this.updateInfo();
        }, 50);
        let snake = this.backend.snakeCreate(this.params.boardSize, 4);
        let brain = this.backend.networkCopy(best);
        let direction = this.makeDecision(snake, brain);
        let snakeLength = 1;
        let life = 100;
        this.chooseSnakeColor();
        this.clearScreen();
        this.drawSnake(snake);
        setInterval(() => {
            life -= 1;
            if (life > 0 && this.backend.snakeMove(snake, direction) > 0) {
                if (this.snakeLength(snake) > snakeLength) {
                    life = 100;
                    snakeLength += 1;
                }
            }
            else {
                this.backend.snakeDelete(snake);
                this.backend.networkDelete(brain);
                snake = this.backend.snakeCreate(this.params.boardSize, 4);
                brain = this.backend.networkCopy(best);
                this.chooseSnakeColor();
                snakeLength = 1;
                life = 100;
            }
            this.clearScreen();
            this.drawSnake(snake);
            direction = this.makeDecision(snake, brain);
        }, 100);
    }
    randomInt(min, max) {
        return Math.floor(Math.random() * (max - min + 1)) + min;
    }
    clearScreen() {
        this.screenContext.fillStyle = "black";
        this.screenContext.fillRect(0, 0, this.params.screen.width, this.params.screen.height);
    }
    drawSnake(pointer) {
        const snakeStruct = new DataView(this.backend.memory.buffer, pointer, 28);
        const body = new ListView_1.ListView(this.backend.memory.buffer, snakeStruct.getUint32(12, true));
        this.screenContext.fillStyle = this.snakeColor;
        body.forEach(partPointer => {
            const part = new DataView(this.backend.memory.buffer, partPointer, 8);
            const x = part.getInt32(0, true) * this.params.blockSize + 2;
            const y = (this.params.boardSize - part.getInt32(4, true) - 1) * this.params.blockSize + 2;
            this.screenContext.fillRect(x, y, this.params.blockSize - 4, this.params.blockSize - 4);
        });
        this.screenContext.fillStyle = "red";
        const foodX = snakeStruct.getInt32(16, true) * this.params.blockSize + 2;
        const foodY = (this.params.boardSize - snakeStruct.getInt32(20, true) - 1) * this.params.blockSize + 2;
        this.screenContext.fillRect(foodX, foodY, this.params.blockSize - 4, this.params.blockSize - 4);
    }
    snakeLength(pointer) {
        const snakeStruct = new DataView(this.backend.memory.buffer, pointer, 28);
        const body = new ListView_1.ListView(this.backend.memory.buffer, snakeStruct.getUint32(12, true));
        return body.size;
    }
    chooseSnakeColor() {
        const r = this.randomInt(127, 255);
        const g = this.randomInt(127, 255);
        const b = this.randomInt(127, 255);
        this.snakeColor = `rgb(${r}, ${g}, ${b})`;
    }
    updateInfo() {
        this.params.networkInput.innerHTML = `Gen.:\n${this.generations}\n\n\n` + this.snakeInfo;
    }
}
exports.App = App;

},{"./ListView":2,"./MatrixView":3}],2:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ListView = void 0;
class ListView {
    constructor(memory, pointer) {
        this.memory = memory;
        this.pointer = pointer;
        this.revalidate();
    }
    revalidate() {
        this.struct = new DataView(this.memory, this.pointer, 12);
    }
    get size() {
        return this.struct.getUint32(0, true);
    }
    forEach(callback) {
        let node = new DataView(this.memory, this.struct.getUint32(4, true), 12);
        node = new DataView(this.memory, node.getUint32(8, true), 12);
        for (let i = 0; node.getUint32(8, true) !== 0; i++) {
            callback(node.getUint32(0, true), i, this);
            node = new DataView(this.memory, node.getUint32(8, true), 12);
        }
    }
}
exports.ListView = ListView;

},{}],3:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.MatrixView = void 0;
class MatrixView {
    constructor(memory, pointer) {
        this.memory = memory;
        this.pointer = pointer;
        this.revalidate();
    }
    revalidate() {
        this.struct = new DataView(this.memory, this.pointer, 16);
    }
    get shape() {
        return [this.struct.getUint32(0, true), this.struct.getUint32(4, true)];
    }
    forEach(callback) {
        const [rows, columns] = this.shape;
        const size = this.struct.getUint32(8, true);
        const values = new Float32Array(this.memory, this.struct.getUint32(12, true), size);
        for (let i = 0, p = 0; i < rows; i++) {
            for (let j = 0; j < columns; j++, p++) {
                callback(values[p], i, j, this);
            }
        }
    }
    print() {
        const [rows, columns] = this.shape;
        const size = this.struct.getUint32(8, true);
        const values = new Float32Array(this.memory, this.struct.getUint32(12, true), size);
        let str = "{";
        for (let i = 0, p = 0; i < rows; i++) {
            str += "{ ";
            for (let j = 0; j < columns; j++, p++) {
                str += `${values[p]}` + (j < columns - 1 ? ", " : " ");
            }
            str += "}" + (i < rows - 1 ? ",\n" : "");
        }
        str += "}";
        console.log(str);
    }
}
exports.MatrixView = MatrixView;

},{}],4:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const App_1 = require("./App");
(async () => {
    const app = new App_1.App({
        screen: document.getElementById("screen"),
        networkInput: document.getElementById("info"),
        boardSize: 15,
        blockSize: 32
    });
    await app.load();
    app.run();
})();

},{"./App":1}]},{},[4]);
