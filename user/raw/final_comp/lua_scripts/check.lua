io.input("test.txt")

num = 4096

for i = 64, 125 do
    truth = string.rep(string.char(i), num)
    hat = io.read()
    if(truth ~= hat) then
        print(string.format("%c panic", i));
        os.exit()
    end
end

io.close()

print("check success!")
