use std::{io::Write, path::PathBuf, process::Command};

use pixelscript::{own_string, shared::{pxs_Runtime, utils::CStringSafe, var::pxs_VarType}};

use crate::{get_result, yoyo::{self, YoyoRes, YoyoResult}, yoyo_dir, yoyo_error};


pub struct Context {
    pub cwd: PathBuf,
    pub target: String,
    pub python_name: String
}

impl Context {
    pub fn new() -> Self {
        Context { cwd: PathBuf::new(), target: String::new(), python_name: String::from("python3") }
    }

    pub fn set_cwd(&mut self, buf: PathBuf) {
        self.cwd = buf;
    }

    pub fn path_buf(&self) -> &PathBuf {
        &self.cwd
    }

}

pub struct Flag {
    pub key: String,
    pub value: String,
}

impl Flag {
    pub fn new(key: String, value: String) -> Self {
        Flag { key, value }
    }

    /// Convert to bool if not true/false/1/0 error
    pub fn to_bool(&self) -> YoyoRes<bool> {
        if self.value == "true" {
            Ok(true)
        } else if self.value == "false" {
            Ok(false)
        } else if self.value == "1" {
            Ok(true)
        } else if self.value == "0" {
            Ok(false)
        } else {
            yoyo_error!("{} is not a boolean", self.key)
        }
    }

    pub fn get_value(&self) -> &String {
        &self.value
    }
    // /// Convert to integer if a number. otherwise error
    // pub fn to_int(&self) -> YoyoRes<i32> {
        // get_result!(self.value.parse::<i32>())
    // }
}

pub struct ArgParser {
    /// The arguments (from cmdline)
    raw_args: Vec<String>,
    /// Specifically flags with their results
    flags: Vec<Flag>,
    /// Generated args without `flags`
    args: Vec<String>,
    /// Current step index in `args`
    step_idx: u32
}

impl ArgParser {
    pub fn new(raw_args: Vec<String>) -> ArgParser {
        ArgParser { raw_args, flags: vec![], args: vec![], step_idx: 0 }
    }

    pub fn parse(&mut self) {
        for i in 0..self.raw_args.len() {
            let arg = &self.raw_args[i];
            if (arg.starts_with("--") || arg.starts_with("-")) && i < self.raw_args.len() - 1 {
                self.flags.push(Flag::new(arg.clone(), self.raw_args[i + 1].clone()));
                continue;
            }

            // Add it to args instead.
            self.args.push(arg.clone());
        }
    }

    pub fn get_flag_keys(&self, poss_keys: Vec<&str>) -> Option<&Flag> {
        for flag in self.flags.iter() {
            for key in poss_keys.iter() {
                if flag.key == *key {
                    return Some(flag);
                }
            }
        }
        None
    }

    /// Get the next argument. If none next, None.
    pub fn next_arg(&mut self) -> Option<String> {
        if self.args.len() == 0 {
            return None;
        }
        if (self.step_idx as usize) > self.args.len() - 1 {
            None
        } else {
            let idx = self.step_idx;
            self.step_idx += 1;
            Some(self.args[idx as usize].clone())
        }
    }
}

/// Compile pixelscript
pub fn pxs(context: &Context, arg_parser: &mut ArgParser) -> YoyoResult {
    let mut current_dir = context.path_buf().clone();
    let should_clear = match arg_parser.next_arg() {
        Some(val) => {
            if val == "clear" {
                "clear"
            } else {
                ""
            }
        },
        None => ""
    };
    // Unzip pixelscript.zip
    let path = context.path_buf();
    let mut path = yoyo_dir!(path);
    yoyo::create_dir(&path)?;
    yoyo::extract_zip_file(path.clone())?;
    path.push("pixelscript");

    let mut script_path = path.clone();
    script_path.push("scripts");
    // Change directories
    get_result!(std::env::set_current_dir(&path))?;
    script_path.push("build.py");

    let status = Command::new(&context.python_name)
        .arg(script_path)
        .arg(should_clear)
        .status();
    let _ = get_result!(status)?;

    // Change back to working directory
    get_result!(std::env::set_current_dir(&current_dir))?;

    path.push("pxsb");
    // Save the binaries into the current target
    current_dir.push("pxsb");
    current_dir.push(&context.target);
    yoyo::create_dir(&current_dir)?;

    // Get files from pxsb
    let files = get_result!(std::fs::read_dir(&path))?;
    for f in files {
        let f = get_result!(f)?;
        let mut new_path = current_dir.clone();
        new_path.push(f.file_name());
        get_result!(std::fs::rename(f.path(), &new_path))?;
    }

    println!("pixelscript binaries have been written to {}", current_dir.display());
    Ok(())
}

/// Run a REPL
pub fn repl(context: &Context, arg_parser: &mut ArgParser) -> YoyoResult {
    let language = arg_parser.next_arg();
    if language.is_none() {
        return yoyo_error!("Expecte language");
    }
    let language = language.unwrap().trim().to_lowercase();

    let runtime = match language.as_str() {
        "python" | "py" => pxs_Runtime::pxs_Python,
        "lua" => pxs_Runtime::pxs_Lua,
        "javascript" | "js" => pxs_Runtime::pxs_JavaScript,
        _ => return yoyo_error!("Unsupported language: {language}.")
    };

    yoyo::start();

    loop {
        let mut cstring = CStringSafe::new();
        print!("> ");
        std::io::stdout().flush().unwrap();
        
        // Get input
        let input = yoyo::get_input();
        let input = input.trim();
        if input.to_lowercase() == "quit" {
            break;
        } else if input.len() == 0 {
            continue;
        }

        // Eval for the runtime
        let result = pixelscript::pxs_exec(runtime.clone(), cstring.new_string(input), cstring.new_string("<repl>"));

        if pixelscript::pxs_varis(result, pxs_VarType::pxs_Exception) {
            let message = own_string!(pixelscript::pxs_getstring(result));
            pixelscript::pxs_freevar(result);
            println!("{message}");
            pixelscript::pxs_clear();
            yoyo::add_modules();
            continue;
        }
        pixelscript::pxs_freevar(result);
    }

    yoyo::stop();

    Ok(())
}