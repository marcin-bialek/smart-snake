export class ListView {
    private memory: ArrayBuffer
    private pointer: number
    private struct: DataView

    public constructor(memory: ArrayBuffer, pointer: number) {
        this.memory = memory
        this.pointer = pointer
        this.revalidate()
    }

    public revalidate() {
        this.struct = new DataView(this.memory, this.pointer, 12);
    }

    public get size(): number {
        return this.struct.getUint32(0, true)
    }

    public forEach(callback: (value: number, index: number, list: ListView) => void) {
        let node = new DataView(this.memory, this.struct.getUint32(4, true), 12)
        node = new DataView(this.memory, node.getUint32(8, true), 12)

        for(let i = 0; node.getUint32(8, true) !== 0; i++) {
            callback(node.getUint32(0, true), i, this)
            node = new DataView(this.memory, node.getUint32(8, true), 12)
        }
    }
}
