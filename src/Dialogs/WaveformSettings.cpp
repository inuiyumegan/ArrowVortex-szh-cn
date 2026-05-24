#include <Dialogs/WaveformSettings.h>

#include <Core/WidgetsLayout.h>

#include <Editor/Waveform.h>

namespace Vortex {

DialogWaveformSettings::~DialogWaveformSettings()
{
}

DialogWaveformSettings::DialogWaveformSettings()
{
	presetIndex_ = 0;

	settingsColorScheme_ = gWaveform->getColors();
	luminanceValue_ = gWaveform->getLuminance();
	waveShape_ = gWaveform->getWaveShape();
	antiAliasingMode_ = gWaveform->getAntiAliasing();
	isOverlayFilterActive_ = gWaveform->getOverlayFilter();

	filterType_ = Waveform::FT_HIGH_PASS;
	filterStrength_ = 0.75f;

	setTitle(Vortex::_TR("WAVEFORM SETTINGS").str());

	myLayout.row().col(96).col(128);

	// Color Scheme.
	WgCycleButton* preset = myLayout.add<WgCycleButton>(Vortex::_TR("Presets").str());
	preset->onChange.bind(this, &DialogWaveformSettings::myApplyPreset);
	preset->value.bind(&presetIndex_);
	preset->addItem(Vortex::_TR("Vortex").str());
	preset->addItem(Vortex::_TR("DDReam").str());
	preset->setTooltip(Vortex::_TR("Preset styles for the waveform appearance").str());

	// BG color.
	bgColor->red.bind(&settingsColorScheme_.bg.r);
	bgColor->green.bind(&settingsColorScheme_.bg.g);
	bgColor->blue.bind(&settingsColorScheme_.bg.b);
	bgColor->alpha.bind(&settingsColorScheme_.bg.a);
	bgColor->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	WgColorPicker* bgColor = myLayout.add<WgColorPicker>(Vortex::_TR("BG color").str());
	bgColor->setTooltip(Vortex::_TR("Color of the waveform background").str());

	// FG color.
	waveColor->red.bind(&settingsColorScheme_.wave.r);
	waveColor->green.bind(&settingsColorScheme_.wave.g);
	waveColor->blue.bind(&settingsColorScheme_.wave.b);
	waveColor->alpha.bind(&settingsColorScheme_.wave.a);
	waveColor->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	WgColorPicker* waveColor = myLayout.add<WgColorPicker>(Vortex::_TR("Wave color").str());
	waveColor->setTooltip(Vortex::_TR("Color of the waveform").str());

	// Filter color
	filterColor->red.bind(&settingsColorScheme_.filter.r);
	filterColor->green.bind(&settingsColorScheme_.filter.g);
	filterColor->blue.bind(&settingsColorScheme_.filter.b);
	filterColor->alpha.bind(&settingsColorScheme_.filter.a);
	filterColor->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	WgColorPicker* filterColor = myLayout.add<WgColorPicker>(Vortex::_TR("Filter color").str());
	filterColor->setTooltip(Vortex::_TR("Color of the filtered waveform").str());

	// Luminance.
	WgCycleButton* lum = myLayout.add<WgCycleButton>(Vortex::_TR("Luminance").str());
	lum->value.bind(&luminanceValue_);
	lum->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	lum->addItem(Vortex::_TR("Uniform").str());
	lum->addItem(Vortex::_TR("Amplitude").str());
	lum->setTooltip(Vortex::_TR("Determines the lightness of the waveform peaks").str());

	// Wave shape.
	WgCycleButton* shape = myLayout.add<WgCycleButton>(Vortex::_TR("Wave shape").str());
	shape->value.bind(&waveShape_);
	shape->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	shape->addItem(Vortex::_TR("Rectified").str());
	shape->addItem(Vortex::_TR("Signed").str());
	shape->setTooltip(Vortex::_TR("Determines the shape of the waveform peaks").str());

	// Anti-aliasing.
	WgCycleButton* aa = myLayout.add<WgCycleButton>(Vortex::_TR("Anti-aliasing").str());
	aa->value.bind(&antiAliasingMode_);
	aa->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	aa->addItem(Vortex::_TR("None").str());
	aa->addItem(Vortex::_TR("2x").str());
	aa->addItem(Vortex::_TR("3x").str());
	aa->addItem(Vortex::_TR("4x").str());
	aa->setTooltip(Vortex::_TR("Determines the smoothness of the waveform shape").str());

	// Filter type.
	myLayout.row().col(228);
	myLayout.add<WgSeperator>();
	myLayout.row().col(96).col(128);

	WgCycleButton* filter = myLayout.add<WgCycleButton>(Vortex::_TR("Filter type").str());
	filter->value.bind(&filterType_);
	filter->addItem(Vortex::_TR("High-pass").str());
	filter->addItem(Vortex::_TR("Low-pass").str());
	filter->setTooltip(Vortex::_TR("Determines the shape of the waveform filter").str());

	// Filter strength.
	WgSlider* strength = myLayout.add<WgSlider>(Vortex::_TR("Strength").str());
	strength->value.bind(&filterStrength_);
	strength->setTooltip(Vortex::_TR("The strength of the waveform filter").str());

	// Show both waveforms.
	myLayout.row().col(228);
	WgCheckbox* bothWaves = myLayout.add<WgCheckbox>();
	bothWaves->text.set(Vortex::_TR("Overlay filtered waveform"));
	bothWaves->value.bind(&isOverlayFilterActive_);
	bothWaves->onChange.bind(this, &DialogWaveformSettings::myToggleOverlayFilter);
	bothWaves->setTooltip(Vortex::_TR("If enabled, the filtered waveform is shown on top of the original waveform").str());

	// Filtering.
	myLayout.row().col(112).col(112);

	WgButton* disable = myLayout.add<WgButton>();
	disable->text.set(Vortex::_TR("Disable filter"));
	disable->onPress.bind(this, &DialogWaveformSettings::myDisableFilter);
	disable->setTooltip(Vortex::_TR("Hides the filtered waveform").str());

	WgButton* enable = myLayout.add<WgButton>();
	enable->text.set(Vortex::_TR("Apply filter"));
	enable->onPress.bind(this, &DialogWaveformSettings::myEnableFilter);
	enable->setTooltip(Vortex::_TR("Shows the filtered waveform").str());
}

void DialogWaveformSettings::myApplyPreset()
{
	gWaveform->setPreset((Waveform::Preset)presetIndex_);
	settingsColorScheme_ = gWaveform->getColors();
	luminanceValue_ = gWaveform->getLuminance();
	waveShape_ = gWaveform->getWaveShape();
	antiAliasingMode_ = gWaveform->getAntiAliasing();
}

void DialogWaveformSettings::myUpdateSettings()
{
	gWaveform->setColors(settingsColorScheme_);
	gWaveform->setAntiAliasing(antiAliasingMode_);
	gWaveform->setLuminance((Waveform::Luminance)luminanceValue_);
	gWaveform->setWaveShape((Waveform::WaveShape)waveShape_);
}

void DialogWaveformSettings::myToggleOverlayFilter()
{
	gWaveform->setOverlayFilter(isOverlayFilterActive_);
}

void DialogWaveformSettings::myEnableFilter()
{
	gWaveform->enableFilter((Waveform::FilterType)filterType_, filterStrength_);
}

void DialogWaveformSettings::myDisableFilter()
{
	gWaveform->disableFilter();
}	

}; // namespace Vortex