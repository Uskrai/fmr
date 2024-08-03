use tracing_subscriber::util::SubscriberInitExt;

fn main() {
    let runtime = tokio::runtime::Builder::new_multi_thread()
        .enable_all()
        .build()
        .unwrap();
    let handle = runtime.handle().clone();
    let _guard = handle.enter();

    let options = eframe::NativeOptions::default();

    // let format = tracing_subscriber::fmt::format()
    //     .pretty()
    //     .with_ansi(false)
    //     .fmt_fields(tracing_subscriber::fmt::FormatFields::new().with_ansi(false));

    tracing_subscriber::fmt()
        .with_env_filter(tracing_subscriber::filter::EnvFilter::from_default_env())
        .pretty()
        // .with_ansi(false)
        // .fmt_fields(tracing_subscriber::fmt)
        // .event_format(format)
        .finish()
        .init();

    tracing::info!("running fmr ({})", git_version::git_version!());

    eframe::run_native(
        "fmr",
        options,
        Box::new(|it| {
            it.egui_ctx.set_visuals(eframe::egui::Visuals::dark());
            Box::new(fmr::App::new(it, runtime))
        }),
    )
    .unwrap();
}
