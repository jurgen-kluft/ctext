package main

import (
	"github.com/jurgen-kluft/xcode"
	"github.com/jurgen-kluft/xtext/package"
)

func main() {
	xcode.Init()
	xcode.Generate(xcrypto.GetPackage())
}
