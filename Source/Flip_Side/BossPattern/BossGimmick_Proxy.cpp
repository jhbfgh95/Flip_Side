#include "BossGimmick_Proxy.h"
#include "BossActor.h"

void UBossGimmick_Proxy::OnPlayerPhaseStart(ABossActor* Boss)
{
    if (!Boss) return;
    for (UBossGimmickBase* G : Boss->GetGimmickList())
    {
        if (IsValid(G))
            G->OnPlayerPhaseStart(Boss);
    }
}

void UBossGimmick_Proxy::OnPlayerPhaseEnd(ABossActor* Boss)
{
    if (!Boss) return;
    for (UBossGimmickBase* G : Boss->GetGimmickList())
    {
        if (IsValid(G))
            G->OnPlayerPhaseEnd(Boss);
    }
}
