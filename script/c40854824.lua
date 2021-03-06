--オーバー・ザ・レインボー
function c40854824.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c40854824.condition)
	e1:SetTarget(c40854824.target)
	e1:SetOperation(c40854824.activate)
	c:RegisterEffect(e1)
	if not c40854824.global_check then
		c40854824.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_CHAIN_SOLVED)
		ge1:SetOperation(c40854824.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c40854824.checkop(e,tp,eg,ep,ev,re,r,rp)
	local code=re:GetHandler():GetOriginalCode()
	if re:IsActiveType(TYPE_MONSTER) and (code==79407975 or code==79856792) then
		Duel.RegisterFlagEffect(rp,40854824,RESET_PHASE+PHASE_END,0,1)
	end
end
function c40854824.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFlagEffect(tp,40854824)~=0
end
function c40854824.filter(c,e,tp)
	return c:IsSetCard(0x1034) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c40854824.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c40854824.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c40854824.activate(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	local g=Duel.GetMatchingGroup(c40854824.filter,tp,LOCATION_DECK,0,nil,e,tp)
	if g:GetCount()==0 then return end
	repeat
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=g:Select(tp,1,1,nil)
		local tc=sg:GetFirst()
		Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP)
		g:Remove(Card.IsCode,nil,tc:GetCode())
		ft=ft-1
	until ft<=0 or g:GetCount()==0 or not Duel.SelectYesNo(tp,aux.Stringid(40854824,1))
	Duel.SpecialSummonComplete()
end
