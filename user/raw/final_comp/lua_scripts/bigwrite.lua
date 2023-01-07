io.output("test.txt")

num = 4096

for i = 64, 125 do
    c = string.char(i)
    j = 0
    while j < num do
        io.write(c)
        j = j + 1
    end
    io.write("\n")
end

io.flush()
io.close()
