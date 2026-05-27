#include <Dialogs/CustomSnap.h>

#include <Core/Draw.h>
#include <Core/WidgetsLayout.h>

#include <Editor/View.h>

#include <Managers/LocaleMan.h>

namespace Vortex {

	DialogCustomSnap::~DialogCustomSnap()
	{
	}

	DialogCustomSnap::DialogCustomSnap()
	{
		myCustomSnap = gView->getCustomSnap();

		setTitle(_TR("CUSTOM SNAP").str());
		myCreateWidgets();
	}

	void DialogCustomSnap::myCreateWidgets()
	{
		myLayout.row().col(80).col(80);

		WgSpinner* scol = myLayout.add<WgSpinner>(_TR("Snapping").str());
		scol->value.bind(&myCustomSnap);
		scol->onChange.bind(this, &DialogCustomSnap::onChange);
		scol->setRange(4.0, 192.0);
		scol->setPrecision(0, 0);
		scol->startCapturingText();
	}

	void DialogCustomSnap::onChange()
	{
		if (myCustomSnap >= 4 && myCustomSnap <= 192)
		{
			gView->setCustomSnap(myCustomSnap);
		}
	}
}; // namespace Vortex