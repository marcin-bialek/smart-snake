export class MatrixView {
    private memory: ArrayBuffer
    private pointer: number
    private struct: DataView

    public constructor(memory: ArrayBuffer, pointer: number) {
        this.memory = memory
        this.pointer = pointer
        this.revalidate()
    }

    public revalidate() {
        this.struct = new DataView(this.memory, this.pointer, 16);
    }

    public get shape(): [number, number] {
        return [this.struct.getUint32(0, true), this.struct.getUint32(4, true)]
    }

    public forEach(callback: (value: number, i: number, j: number, matrix: MatrixView) => void) {
        const [rows, columns] = this.shape
        const size = this.struct.getUint32(8, true)
        const values = new Float32Array(this.memory, this.struct.getUint32(12, true), size)
        
        for(let i = 0, p = 0; i < rows; i++) {
            for(let j = 0; j < columns; j++, p++) {
                callback(values[p], i, j, this)
            }
        }
    }

    public print() {
        const [rows, columns] = this.shape
        const size = this.struct.getUint32(8, true)
        const values = new Float32Array(this.memory, this.struct.getUint32(12, true), size)
        
        let str = "{"
        
        for(let i = 0, p = 0; i < rows; i++) {
            str += "{ "
    
            for(let j = 0; j < columns; j++, p++) {
                str += `${values[p]}` + (j < columns - 1 ? ", " : " ")
            }
    
            str += "}" + (i < rows - 1 ? ",\n" : "")
        }
    
        str += "}"
        console.log(str)
    }
}
