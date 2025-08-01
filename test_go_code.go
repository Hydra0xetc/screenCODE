package main

import "fmt"

// This is a comment
func main() {
    fmt.Println("Hello, Go!")
    myFunction()
}

func myFunction() {
    // Another comment
    s := "This is a string"
    fmt.Println(s)
    i := 42
    fmt.Println(i)
}
