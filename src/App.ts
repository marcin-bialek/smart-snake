import { ListView } from "./ListView"
import { MatrixView } from "./MatrixView"


export interface AppParams {
    screen: HTMLCanvasElement
    boardSize: number
    blockSize: number
    networkInput: HTMLElement
}


export class App {
    private params: AppParams
    private screenContext: CanvasRenderingContext2D
    private backend: {
        memory: WebAssembly.Memory
        [functions: string]: any
    }
    private snakeColor = ""
    private generations = 0
    private snakeInfo = ""

    public constructor(params: AppParams) {
        this.params = params
        this.screenContext = params.screen.getContext("2d")
        params.screen.width = params.blockSize * params.boardSize
        params.screen.height = params.screen.width
    }

    public async load() {
        const module = await fetch("backend.wasm")
        const buffer = await module.arrayBuffer()
        const instance = await WebAssembly.instantiate(buffer, {
            env: {
                randomFloat: Math.random,
                randomInt: this.randomInt
            }
        })
        this.backend = <any>instance.instance.exports
    }

    private makeDecision(snake: any, brain: any): number {
        const distanceMatrix = this.backend.snakeCalcDistanceMatrix(snake)
        const y = this.backend.networkFit(brain, distanceMatrix)
        let direction = 0
        let value = 0

        new MatrixView(this.backend.memory.buffer, y).forEach((v, i) => {
            if(v > value) {
                value = v 
                direction = i 
            }
        })

        let str = "In:\n"
        new MatrixView(this.backend.memory.buffer, distanceMatrix).forEach(v => {
            str += `${Math.round(100 * v)/100}\n`
        })
        this.snakeInfo = str + "\n\nOut:\n" + ["up", "right", "down", "left"][direction]
        this.updateInfo()

        this.backend.matrixDelete(distanceMatrix)
        this.backend.matrixDelete(y)

        return direction
    }

    public run() {
        const evolution = this.backend.jsEvolutionCreate()
        this.backend.evolutionInit(evolution)
        let best = this.backend.evolutionGetBestNetwork(evolution)

        setInterval(() => {
            this.backend.evolutionNextGeneration(evolution)
            best = this.backend.evolutionGetBestNetwork(evolution)
            this.generations += 1
            this.updateInfo()
        }, 50)

        let snake = this.backend.snakeCreate(this.params.boardSize, 4)
        let brain = this.backend.networkCopy(best)
        let direction = this.makeDecision(snake, brain)
        let snakeLength = 1
        let life = 100
        this.chooseSnakeColor()
        this.clearScreen()
        this.drawSnake(snake)

        setInterval(() => {
            life -= 1 

            if(life > 0 && this.backend.snakeMove(snake, direction) > 0) {
                if(this.snakeLength(snake) > snakeLength) {
                    life = 100
                    snakeLength += 1
                }
            }
            else {
                this.backend.snakeDelete(snake)
                this.backend.networkDelete(brain)
                snake = this.backend.snakeCreate(this.params.boardSize, 4)
                brain = this.backend.networkCopy(best)
                this.chooseSnakeColor()
                snakeLength = 1
                life = 100
            }

            this.clearScreen()
            this.drawSnake(snake)
            direction = this.makeDecision(snake, brain)
        }, 100)
    }

    private randomInt(min: number, max: number): number {
        return Math.floor(Math.random() * (max - min + 1)) + min
    }

    private clearScreen() {
        this.screenContext.fillStyle = "black"
        this.screenContext.fillRect(0, 0, this.params.screen.width, this.params.screen.height)
    }

    private drawSnake(pointer: number) {
        const snakeStruct = new DataView(this.backend.memory.buffer, pointer, 28)
        const body = new ListView(this.backend.memory.buffer, snakeStruct.getUint32(12, true))
        this.screenContext.fillStyle = this.snakeColor

        body.forEach(partPointer => {
            const part = new DataView(this.backend.memory.buffer, partPointer, 8)
            const x = part.getInt32(0, true) * this.params.blockSize + 2
            const y = (this.params.boardSize - part.getInt32(4, true) - 1) * this.params.blockSize + 2
            this.screenContext.fillRect(x, y, this.params.blockSize - 4, this.params.blockSize - 4)
        })

        this.screenContext.fillStyle = "red"
        const foodX = snakeStruct.getInt32(16, true) * this.params.blockSize + 2
        const foodY = (this.params.boardSize - snakeStruct.getInt32(20, true) - 1) * this.params.blockSize + 2
        this.screenContext.fillRect(foodX, foodY, this.params.blockSize - 4, this.params.blockSize - 4)
    }

    private snakeLength(pointer: number) {
        const snakeStruct = new DataView(this.backend.memory.buffer, pointer, 28)
        const body = new ListView(this.backend.memory.buffer, snakeStruct.getUint32(12, true))
        return body.size
    }

    private chooseSnakeColor() {
        const r = this.randomInt(127, 255)
        const g = this.randomInt(127, 255)
        const b = this.randomInt(127, 255)
        this.snakeColor = `rgb(${ r }, ${ g }, ${ b })`
    }

    private updateInfo() {
        this.params.networkInput.innerHTML = `Gen.:\n${this.generations}\n\n\n` + this.snakeInfo
    }
}
