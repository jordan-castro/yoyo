class Flag:
    """Hold Flag types (-)(--)"""
    def __init__(self, key, value):
        self.key = key
        self.value = value
    
    def get_value(self):
        """Get the value of a flag."""
        return self.value


class ArgParser:
    """Parse command line arguments."""
    def __init__(self, args):
        self.raw_args = args
        self.flags = []
        self.args = []
        self.step_idx = 0

    def parse(self):
        """Parse the ArgParser"""
        argc = len(self.raw_args)
        for i in range(argc):
            arg: str = self.raw_args[i]
            if arg.startswith('--') or arg.startswith('-') and i < argc - 1:
                self.flags.append(Flag(arg, self.raw_args[i + 1]))
                continue
            # Add to args instaed
            self.args.append(arg)

    def get_flag(self, keys: list[str]) -> Flag:
        """Get a flag based on a list of keys."""
        for flag in self.flags:
            for key in keys:
                if flag.key == key:
                    return flag
        
        return None

    def next_arg(self) -> str:
        """Get the current argument and move the cursor."""
        argc = len(self.args)
        if argc == 0:
            return None
    
        if self.step_idx > argc - 1:
            return None
    
        idx = self.step_idx
        self.step_idx += 1
        return self.args[idx]

    def current_arg(self) -> str:
        """Get current argument without moving the cursor."""
        if len(self.args) == 0:
            return None
    
        if self.step_idx == 0:
            return self.args[0]
        else:
            return self.args[self.step_idx - 1]
