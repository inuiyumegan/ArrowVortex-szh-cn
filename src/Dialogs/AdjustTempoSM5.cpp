#include <Dialogs/AdjustTempoSM5.h>

#include <Core/Widgets.h>
#include <Core/WidgetsLayout.h>

#include <Managers/TempoMan.h>
#include <Managers/SimfileMan.h>

#include <Simfile/SegmentGroup.h>

#include <Editor/Selection.h>
#include <Editor/View.h>
#include <Editor/History.h>
#include <Editor/Common.h>
#include <Editor/Editing.h>

#include <Managers/LocaleMan.h>

namespace Vortex {

enum Actions
{
	ACT_DELAY_SET,
	ACT_WARP_SET,
	ACT_TIME_SIG_SET,
	ACT_TICK_COUNT_SET,
	ACT_COMBO_SET,
	ACT_SPEED_SET,
	ACT_SCROLL_SET,
	ACT_FAKE_SET,
	ACT_LABEL_SET,
};

DialogAdjustTempoSM5::~DialogAdjustTempoSM5()
{
}

DialogAdjustTempoSM5::DialogAdjustTempoSM5()
{
	setTitle(_TR("ADJUST TEMPO (SM5)").str());
	myCreateWidgets();
	onChanges(VCM_ALL_CHANGES);
	clear();
}

void DialogAdjustTempoSM5::clear()
{
	myDelay = 0.0;
	myWarp = 0.0;
	myTimeSigBpm = 4, myTimeSigNote = 4;
	myTickCount = 0;
	myComboHit = 1, myComboMiss = 1;
	mySpeedRatio = 1.0, mySpeedDelay = 0.0;
	mySpeedUnit = 0;
	myScrollRatio = 1.0;
	myFakeBeats = 0.0;
	myLabelText.clear();
}

void DialogAdjustTempoSM5::myCreateWidgets()
{
	myLayout.row().col(84).col(154);

	WgSpinner* spinner = myLayout.add<WgSpinner>(_TR("Delay").str());
	spinner->value.bind(&myDelay);
	spinner->setPrecision(3, 6);
	spinner->setStep(0.001);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_DELAY_SET);
	spinner->setTooltip(_TR("Stop length at the current beat, in seconds").str());

	myLayout.row().col(84).col(154);
	spinner = myLayout.add<WgSpinner>(_TR("Warp").str());
	spinner->value.bind(&myWarp);
	spinner->setPrecision(3, 6);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_WARP_SET);
	spinner->setTooltip(_TR("Warp length at the current beat, in beats").str());

	myLayout.row().col(84).col(75).col(75);

	spinner = myLayout.add<WgSpinner>(_TR("Time sig.").str());
	spinner->value.bind(&myTimeSigBpm);
	spinner->setPrecision(0, 0);
	spinner->setRange(1, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_TIME_SIG_SET);
	spinner->setTooltip(_TR("Beats per measure").str());

	spinner = myLayout.add<WgSpinner>();
	spinner->value.bind(&myTimeSigNote);
	spinner->setPrecision(0, 0);
	spinner->setRange(1, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_TIME_SIG_SET);
	spinner->setTooltip(_TR("Beat note type").str());

	myLayout.row().col(84).col(154);

	spinner = myLayout.add<WgSpinner>(_TR("Ticks").str());
	spinner->value.bind(&myTickCount);
	spinner->setPrecision(0, 0);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_TICK_COUNT_SET);
	spinner->setTooltip(_TR("Hold combo ticks per beat").str());

	myLayout.row().col(84).col(75).col(75);

	spinner = myLayout.add<WgSpinner>(_TR("Combo").str());
	spinner->value.bind(&myComboHit);
	spinner->setPrecision(0, 0);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_COMBO_SET);
	spinner->setTooltip(_TR("Combo hit multiplier").str());

	spinner = myLayout.add<WgSpinner>();
	spinner->value.bind(&myComboMiss);
	spinner->setPrecision(0, 0);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_COMBO_SET);
	spinner->setTooltip(_TR("Combo miss multiplier").str());

	myLayout.row().col(84).col(52).col(56).col(38);

	spinner = myLayout.add<WgSpinner>(_TR("Speed").str());
	spinner->value.bind(&mySpeedRatio);
	spinner->setPrecision(2, 6);
	spinner->setStep(0.1);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_SPEED_SET);
	spinner->setTooltip(_TR("Stretch ratio").str());

	spinner = myLayout.add<WgSpinner>();
	spinner->value.bind(&mySpeedDelay);
	spinner->setPrecision(2, 6);
	spinner->setStep(0.1);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_SPEED_SET);
	spinner->setTooltip(_TR("Delay time").str());

	WgCycleButton* cycler = myLayout.add<WgCycleButton>();
	cycler->value.bind(&mySpeedUnit);
	cycler->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_SPEED_SET);
	cycler->setTooltip(_TR("Delay unit (beats/time)").str());
	cycler->addItem("B");
	cycler->addItem("T");

	myLayout.row().col(84).col(154);

	spinner = myLayout.add<WgSpinner>(_TR("Scroll").str());
	spinner->value.bind(&myScrollRatio);
	spinner->setPrecision(2, 6);
	spinner->setStep(0.1);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_SCROLL_SET);
	spinner->setTooltip(_TR("Scroll ratio").str());

	spinner = myLayout.add<WgSpinner>(_TR("Fakes").str());
	spinner->value.bind(&myFakeBeats);
	spinner->setPrecision(3, 6);
	spinner->setRange(0, 1000);
	spinner->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_FAKE_SET);
	spinner->setTooltip(_TR("Fake region, in beats").str());

	WgLineEdit* text = myLayout.add<WgLineEdit>(_TR("Label").str());
	text->text.bind(&myLabelText);
	text->setMaxLength(1000);
	text->onChange.bind(this, &DialogAdjustTempoSM5::onAction, (int)ACT_LABEL_SET);
	text->setTooltip(_TR("Label text").str());
}

void DialogAdjustTempoSM5::onChanges(int changes)
{
	if(changes & VCM_FILE_CHANGED)
	{
		clear();
		if(gSimfile->isOpen())
		{
			for(auto w : myLayout) w->setEnabled(true);
		}
		else
		{
			for(auto w : myLayout) w->setEnabled(false);
		}
	}
}

void DialogAdjustTempoSM5::onTick()
{
	if(gSimfile->isOpen())
	{
		int row = gView->getCursorRow();
		auto segs = gTempo->getSegments();

		myDelay = segs->getRow<Delay>(row).seconds;
		myWarp = segs->getRow<Warp>(row).numRows / (double)ROWS_PER_BEAT;
		
		auto sig = segs->getRecent<TimeSignature>(row);
		myTimeSigBpm = sig.rowsPerMeasure / ROWS_PER_BEAT;
		myTimeSigNote = sig.beatNote;

		myTickCount = segs->getRecent<TickCount>(row).ticks;

		auto combo = segs->getRecent<Combo>(row);
		myComboHit = combo.hitCombo;
		myComboMiss = combo.missCombo;

		auto speed = segs->getRecent<Speed>(row);
		mySpeedRatio = speed.ratio;
		mySpeedDelay = speed.delay;
		mySpeedUnit = speed.unit;

		myScrollRatio = segs->getRecent<Scroll>(row).ratio;
		myFakeBeats = segs->getRow<Fake>(row).numRows / (double)ROWS_PER_BEAT;

		myLabelText = segs->getRow<Label>(row).str;
	}
	EditorDialog::onTick();
}

void DialogAdjustTempoSM5::onAction(int id)
{
	if(gSimfile->isClosed()) return;
	int row = gView->getCursorRow();
	switch(id)
	{
	case ACT_DELAY_SET: {
		gTempo->addSegment(Delay(row, myDelay));
	} break;
	case ACT_WARP_SET: {
		int rows = max(0, (int)(ROWS_PER_BEAT * myWarp));
		gTempo->addSegment(Warp(row, rows));
	} break;
	case ACT_TIME_SIG_SET: {
		int rowsPerMeasure = ROWS_PER_BEAT * max(1, (int)myTimeSigBpm);
		int beatNote = max(1, myTimeSigNote);
		gTempo->addSegment(TimeSignature(row, rowsPerMeasure, beatNote));
	} break;
	case ACT_TICK_COUNT_SET: {
		int ticks = max(0, myTickCount);
		gTempo->addSegment(TickCount(row, ticks));
	} break;
	case ACT_COMBO_SET: {
		int hit = max(1, myComboHit);
		int miss = max(1, myComboMiss);
		gTempo->addSegment(Combo(row, hit, miss));
	} break;
	case ACT_SPEED_SET: {
		double ratio = max(0.0, mySpeedRatio);
		double delay = max(0.0, mySpeedDelay);
		int unit = clamp(mySpeedUnit, 0, 1);
		gTempo->addSegment(Speed(row, ratio, delay, unit));
	} break;
	case ACT_SCROLL_SET: {
		double ratio = max(0.0, myScrollRatio);
		gTempo->addSegment(Scroll(row, ratio));
	} break;
	case ACT_FAKE_SET: {
		int rows = max(0, (int)(ROWS_PER_BEAT * myFakeBeats));
		gTempo->addSegment(Fake(row, rows));
	} break;
	case ACT_LABEL_SET: {
		gTempo->addSegment(Label(row, myLabelText));
	} break;
	};
}

}; // namespace Vortex
