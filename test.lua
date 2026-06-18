local yoyo = require("yoyo")

yoyo.print('YOYO lua!')

local Person = {}
Person.__index = Person

function Person.new(name, age)
    local instance = setmetatable({}, Person)
    instance.name = name
    instance.age = age

    return instance
end

function Person:greet(other)
    yoyo.print("Hello " .. other.name .. "! My name is " .. self.name .. ". And I am " .. tostring(self.age) .. " years old.")
end

local jordan = Person.new("jordan", 24)

local evelyn = Person.new("Evelyn", 21)

jordan:greet(evelyn)
evelyn:greet(jordan)