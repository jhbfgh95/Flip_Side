#include "BossGimmick_Proxy.h"
#include "BossActor.h"

void UBossGimmick_Proxy::OnPlayerTurnStart(ABossActor* Boss)
{
    if (!Boss) return;
    for (UBossGimmickBase* G : Boss->GetGimmickList())
    {
        if (IsValid(G))
            G->OnPlayerTurnStart(Boss);
    }
}

void UBossGimmick_Proxy::OnPlayerTurnEnd(ABossActor* Boss)
{
    if (!Boss) return;
    for (UBossGimmickBase* G : Boss->GetGimmickList())
    {
        if (IsValid(G))
            G->OnPlayerTurnEnd(Boss);
    }
}
