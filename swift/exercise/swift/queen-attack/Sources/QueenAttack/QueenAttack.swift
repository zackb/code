//Solution goes in Sources

struct Queens: CustomStringConvertible {
    
    private static let Size = 8
    
    enum InitError: Error {
        case invalidCoordinates
        case incorrectNumberOfCoordinates
        case sameSpace
    }

    var white: [Int] = [0, 3]
    var black: [Int] = [7, 8]

    init(white: [Int] = [0, 3], black: [Int] = [7, 3]) throws {
        if white.count + black.count != 4 {
            throw InitError.incorrectNumberOfCoordinates
        }

        if ((white + black).first(where: { $0 >= Queens.Size || $0 < 0}) != nil) {
            throw InitError.invalidCoordinates
        }
        
        if white == black {
            throw InitError.sameSpace
        }
        
        self.white = white
        self.black = black
    }
    
    
    var canAttack: Bool {
        return white[0] == black[0] || white[1] == black[1] ||
            abs(white[0] - black[0]) == abs(white[1] - black[1])
    }
    
    var description: String {
        var d = ""
        for i in 0..<Queens.Size {
            for j in 0..<Queens.Size {
                if white == [i, j] {
                    d += "W"
                } else if black == [i, j] {
                    d += "B"
                } else {
                    d += "_"
                }
                if j < Queens.Size - 1 {
                    d += " "
                }
            }
            if i < Queens.Size - 1 {
                d += "\n"
            }
        }
        return d
    }
}
