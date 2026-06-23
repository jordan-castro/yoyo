pub type YResult<T> = Result<T, String>;

#[macro_export]
macro_rules! unwrap_result {
    ($result:expr) => {
        match $result {
            Ok(val) => val,
            Err(err) => return Err(err.to_string())
        }
    };
}