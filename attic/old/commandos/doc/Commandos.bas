Attribute VB_Name = "Module1"
' Commandos Pack Format

Type CommandosPack
        Name   As String * 32
        Type   As Byte

        Ignore1 As Byte
        Ignore2 As Byte
        Ignore3 As Byte
        
        Size    As Long
        Offset  As Long
End Type

Function OpenFile(ByVal FileName As String)
Dim Pack(1 To 1000) As CommandosPack

Open FileName For Binary As #1
Get #1, , Pack()

Dim Buffer As String
Buffer = Space(Pack(2).Size)
Get #1, Pack(2).Offset, Buffer
Close #1

Open App.Path + "\This_Is_A_Test.txt" For Binary As #1
Put #1, , Buffer
Close #1

End Function
