#include "axisPosData.h"

size_t LUNOBackend::AxisPosData::numAxisType(LUNOBackend::AXIS_TYPE at)
{
	size_t nn = 0;

	for (auto it = m_AxisData.begin(); it != m_AxisData.end(); ++it)
	{
		if (it->getAxisType() == at)
			nn++;
	}

	return nn;
}
