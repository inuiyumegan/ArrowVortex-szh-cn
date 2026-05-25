#include <Dialogs/WaveformSettings.h>

#include <Core/WidgetsLayout.h>

#include <Editor/Waveform.h>

#include <Managers/LocaleMan.h>

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

	setTitle(_TR("WAVEFORM SETTINGS").str());

	myLayout.row().col(96).col(128);

	// Color Scheme.
	WgCycleButton* preset = myLayout.add<WgCycleButton>(_TR("Presets").str());
	preset->onChange.bind(this, &DialogWaveformSettings::myApplyPreset);
	preset->value.bind(&presetIndex_);
	preset->addItem(_TR("Vortex").str());
	preset->addItem(_TR("DDReam").str());
	preset->setTooltip(_TR("Preset styles for the waveform appearance").str());

	// BG color.
	WgColorPicker* bgColor = myLayout.add<WgColorPicker>(_TR("BG color").str());
	bgColor->red.bind(&settingsColorScheme_.bg.r);
	bgColor->green.bind(&settingsColorScheme_.bg.g);
	bgColor->blue.bind(&settingsColorScheme_.bg.b);
	bgColor->alpha.bind(&settingsColorScheme_.bg.a);
	bgColor->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	bgColor->setTooltip(_TR("Color of the waveform background").str());

	// FG color.
	WgColorPicker* waveColor = myLayout.add<WgColorPicker>(_TR("Wave color").str());
	waveColor->red.bind(&settingsColorScheme_.wave.r);
	waveColor->green.bind(&settingsColorScheme_.wave.g);
	waveColor->blue.bind(&settingsColorScheme_.wave.b);
	waveColor->alpha.bind(&settingsColorScheme_.wave.a);
	waveColor->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	waveColor->setTooltip(_TR("Color of the waveform").str());

	// Filter color
	WgColorPicker* filterColor = myLayout.add<WgColorPicker>(_TR("Filter color").str());
	filterColor->red.bind(&settingsColorScheme_.filter.r);
	filterColor->green.bind(&settingsColorScheme_.filter.g);
	filterColor->blue.bind(&settingsColorScheme_.filter.b);
	filterColor->alpha.bind(&settingsColorScheme_.filter.a);
	filterColor->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	filterColor->setTooltip(_TR("Color of the filtered waveform").str());

	// Luminance.
	WgCycleButton* lum = myLayout.add<WgCycleButton>(_TR("Luminance").str());
	lum->value.bind(&luminanceValue_);
	lum->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	lum->addItem(_TR("Uniform").str());
	lum->addItem(_TR("Amplitude").str());
	lum->setTooltip(_TR("Determines the lightness of the waveform peaks").str());

	// Wave shape.
	WgCycleButton* shape = myLayout.add<WgCycleButton>(_TR("Wave shape").str());
	shape->value.bind(&waveShape_);
	shape->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	shape->addItem(_TR("Rectified").str());
	shape->addItem(_TR("Signed").str());
	shape->setTooltip(_TR("Determines the shape of the waveform peaks").str());

	// Anti-aliasing.
	WgCycleButton* aa = myLayout.add<WgCycleButton>(_TR("Anti-aliasing").str());
	aa->value.bind(&antiAliasingMode_);
	aa->onChange.bind(this, &DialogWaveformSettings::myUpdateSettings);
	aa->addItem(_TR("None").str());
	aa->addItem(_TR("2x").str());
	aa->addItem(_TR("3x").str());
	aa->addItem(_TR("4x").str());
	aa->setTooltip(_TR("Determines the smoothness of the waveform shape").str());

	// Filter type.
	myLayout.row().col(228);
	myLayout.add<WgSeperator>();
	myLayout.row().col(96).col(128);

	WgCycleButton* filter = myLayout.add<WgCycleButton>(_TR("Filter type").str());
	filter->value.bind(&filterType_);
	filter->addItem(_TR("High-pass").str());
	filter->addItem(_TR("Low-pass").str());
	filter->setTooltip(_TR("Determines the shape of the waveform filter").str());

	// Filter strength.
	WgSlider* strength = myLayout.add<WgSlider>(_TR("Strength").str());
	strength->value.bind(&filterStrength_);
	strength->setTooltip(_TR("The strength of the waveform filter").str());

	// Show both waveforms.
	myLayout.row().col(228);
	WgCheckbox* bothWaves = myLayout.add<WgCheckbox>();
	bothWaves->text.set(_TR("Overlay filtered waveform").str());
	bothWaves->value.bind(&isOverlayFilterActive_);
	bothWaves->onChange.bind(this, &DialogWaveformSettings::myToggleOverlayFilter);
	bothWaves->setTooltip(_TR("If enabled, the filtered waveform is shown on top of the original waveform").str());

	// Filtering.
	myLayout.row().col(112).col(112);

	WgButton* disable = myLayout.add<WgButton>();
	disable->text.set(_TR("Disable filter").str());
	disable->onPress.bind(this, &DialogWaveformSettings::myDisableFilter);
	disable->setTooltip(_TR("Hides the filtered waveform").str());

	WgButton* enable = myLayout.add<WgButton>();
	enable->text.set(_TR("Apply filter").str());
	enable->onPress.bind(this, &DialogWaveformSettings::myEnableFilter);
	enable->setTooltip(_TR("Shows the filtered waveform").str());
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