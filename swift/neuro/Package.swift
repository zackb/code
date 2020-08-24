// swift-tools-version:5.1

import PackageDescription

let package = Package(
    name: "neuro",
    products: [
        .executable(name: "neuro", targets: ["neuro"])
    ],
    dependencies: [
        // .package(url: "https://github.com/BiAtoms/Http.swift.git", from: "2.2.1")
        .package(url: "https://github.com/httpswift/swifter.git", .upToNextMajor(from: "1.4.7"))
    ],
    targets: [
        .target(
            name: "neuro",
            dependencies: ["Swifter"]),
        .testTarget(
            name: "neuroTests",
            dependencies: ["neuro"]),
    ]
)
