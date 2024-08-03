pub fn retains<F>(ctx: &egui::Context, f: F)
where
    F: FnMut(&egui::Event) -> bool,
{
    let results = ctx.input(|it| it.events.clone());
    let results: Vec<_> = results.iter().map(f).collect();
    let mut results = results.into_iter();

    ctx.input_mut(|it| it.events.retain(|_| results.next().unwrap_or(true)))
}

pub fn retains_ui<F>(ctx: &mut egui::Ui, mut f: F)
where
    F: FnMut(&mut egui::Ui, &egui::Event) -> bool,
{
    retains(&ctx.ctx().clone(), |it| f(ctx, it))
}

pub fn handles_ui<F>(ctx: &mut egui::Ui, mut f: F)
where
    F: FnMut(&mut egui::Ui, &egui::Event) -> bool,
{
    handles(&ctx.ctx().clone(), |event| f(ctx, event))
}

pub fn handles<F>(ctx: &egui::Context, mut f: F)
where
    F: FnMut(&egui::Event) -> bool,
{
    retains(ctx, |it| !f(it))
}
