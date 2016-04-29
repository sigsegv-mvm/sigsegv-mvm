#include "re/nextbot.h"


void IHotplugAction::UnloadAll()
{
	while (!AutoList<IHotplugAction>::List().empty()) {
		auto action = AutoList<IHotplugAction>::List().front();
		
		auto behavior = action->GetBehavior();
		auto main_action = static_cast<Action<CTFBot> *>(behavior->FirstContainedResponder());
		if (main_action != nullptr) {
			main_action->InvokeOnEnd(action->GetActor(), behavior, nullptr);
			delete main_action;
			behavior->m_MainAction = nullptr;
		}
	}
}
