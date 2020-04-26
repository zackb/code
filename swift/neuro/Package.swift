// swift-tools-version:5.1

import PackageDescription

let package = Package(
    name: "neuro",
    dependencies: [
        .package(url: "https://github.com/BiAtoms/Http.swift.git", from: "2.2.1")
    ],
    targets: [
        .target(
            name: "neuro",
            dependencies: ["HttpSwift"]),
        .testTarget(
            name: "neuroTests",
            dependencies: ["neuro"]),
    ]
)
