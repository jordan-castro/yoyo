//! The API for working with yoyo without it's runtime.
//! Think "yoyo.os", "yoyo.io", "yoyo.cross" Without being forced to use Yoyo.
const std = @import("std");

/// String type
pub const YoyoString = struct {
    contents: []u8,
    allocator: std.mem.Allocator,

    pub fn push_raw(self: *YoyoString, str: []const u8) void {
        const csize = self.contents.len;
        self.allocator.free(self.contents);
        self.contents = self.allocator.alloc([]8, csize + str.len);
        
    }
};