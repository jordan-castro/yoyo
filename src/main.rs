use std::{io::{Cursor, Write}, path::PathBuf};

use pixelscript::{own_string, pxs_clear, pxs_debugvar, pxs_eval, pxs_exec, pxs_freevar, pxs_getstring, pxs_tostring, pxs_varis, shared::{PtrMagic, pxs_Runtime, utils::CStringSafe, var::pxs_VarType}};
use zip::ZipArchive;

use crate::{commands::{ArgParser, Context, pxs, repl}, yoyo::YoyoResult};

mod yoyo;
mod commands;

const USAGE: &str = r#"YoYo v0.0.1
-h, --help          Get this message.

init <name>
compile <main_file>
"#;


fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut context = Context::new();

    if args.len() < 2 {
        println!("{USAGE}");
        return;
    }

    let mut arg_parser = ArgParser::new(args);
    arg_parser.parse();
    // skip yoyo
    arg_parser.next_arg();

    let cwd = arg_parser.get_flag_keys(vec!["--path", "-p"]);
    if let Some(cwd) = cwd {
        context.set_cwd(PathBuf::from(cwd.get_value()));
    } else {
        context.set_cwd(std::env::current_dir().unwrap());
    }

    let python = arg_parser.get_flag_keys(vec!["--python", "-py"]);
    if let Some(python) = python {
        context.python_name = python.get_value().clone();
    }

    let target = arg_parser.get_flag_keys(vec!["--target"]);
    if let Some(target) = target {
        context.target = target.get_value().clone();
    } else {
        context.target = env!("TARGET").to_string();
    }

    let command = arg_parser.next_arg();
    println!("command: {:#?}", command);
    if let Some(command) = command {
        let result = if command == "pxs" {
            pxs(&context, &mut arg_parser)
        } else if command == "repl" {
            repl(&context, &mut arg_parser)
        } else {
            println!("{USAGE}");
            return;
        };

        match result {
            Ok(_) => {},
            Err(err) => println!("{err}"),
        }
    } else {
        println!("{USAGE}");
    }
}

