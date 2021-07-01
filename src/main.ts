import { App } from "./App"


;(async () => {
    const app = new App({
        screen: <HTMLCanvasElement>document.getElementById("screen"),
        networkInput: document.getElementById("info"),
        boardSize: 15,
        blockSize: 32
    })

    await app.load()
    app.run()
})()
