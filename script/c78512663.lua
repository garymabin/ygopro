--E·HERO マグマ·ネオス
function c78512663.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCode3(c,89943723,89621922,80344569,false,false)
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c78512663.splimit)
	c:RegisterEffect(e1)
	--special summon rule
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_EXTRA)
	e2:SetCondition(c78512663.spcon)
	e2:SetOperation(c78512663.spop)
	c:RegisterEffect(e2)
	--return
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(78512663,0))
	e3:SetCategory(CATEGORY_TODECK)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c78512663.retcon1)
	e3:SetTarget(c78512663.rettg)
	e3:SetOperation(c78512663.retop)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e4:SetProperty(0)
	e4:SetCondition(c78512663.retcon2)
	c:RegisterEffect(e4)
	--atkup
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e5:SetCode(EFFECT_UPDATE_ATTACK)
	e5:SetRange(LOCATION_MZONE)
	e5:SetValue(c78512663.atkval)
	c:RegisterEffect(e5)
	--tohand
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(78512663,1))
	e6:SetCategory(CATEGORY_TOHAND)
	e6:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e6:SetCode(78512663)
	e6:SetTarget(c78512663.thtg)
	e6:SetOperation(c78512663.thop)
	c:RegisterEffect(e6)
end
function c78512663.splimit(e,se,sp,st)
	return not e:GetHandler():IsLocation(LOCATION_EXTRA)
end
function c78512663.spfilter(c,code)
	if c:GetCode()~=code then return false end
	if c:IsType(TYPE_FUSION) then return c:IsAbleToExtraAsCost()
	else return c:IsAbleToDeckAsCost() end
end
function c78512663.spcon(e,c)
	if c==nil then return true end 
	local tp=c:GetControler()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<-2 then return false end
	local g1=Duel.GetMatchingGroup(c78512663.spfilter,tp,LOCATION_ONFIELD,0,nil,89943723)
	local g2=Duel.GetMatchingGroup(c78512663.spfilter,tp,LOCATION_ONFIELD,0,nil,89621922)
	local g3=Duel.GetMatchingGroup(c78512663.spfilter,tp,LOCATION_ONFIELD,0,nil,80344569)
	if g1:GetCount()==0 or g2:GetCount()==0 or g3:GetCount()==0 then return false end
	if ft>0 then return true end
	local f1=g1:FilterCount(Card.IsLocation,nil,LOCATION_MZONE)>0 and 1 or 0
	local f2=g2:FilterCount(Card.IsLocation,nil,LOCATION_MZONE)>0 and 1 or 0
	local f3=g3:FilterCount(Card.IsLocation,nil,LOCATION_MZONE)>0 and 1 or 0
	if ft==-2 then return f1+f2+f3==3
	elseif ft==-1 then return f1+f2+f3>=2
	else return f1+f2+f3>=1 end
end
function c78512663.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local g1=Duel.GetMatchingGroup(c78512663.spfilter,tp,LOCATION_ONFIELD,0,nil,89943723)
	local g2=Duel.GetMatchingGroup(c78512663.spfilter,tp,LOCATION_ONFIELD,0,nil,89621922)
	local g3=Duel.GetMatchingGroup(c78512663.spfilter,tp,LOCATION_ONFIELD,0,nil,80344569)
	g1:Merge(g2)
	g1:Merge(g3)
	local g=Group.CreateGroup()
	local tc=nil
	for i=1,3 do
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		if ft<=0 then
			tc=g1:FilterSelect(tp,Card.IsLocation,1,1,nil,LOCATION_MZONE):GetFirst()
		else
			tc=g1:Select(tp,1,1,nil):GetFirst()
		end
		g:AddCard(tc)
		g1:Remove(Card.IsCode,nil,tc:GetCode())
		ft=ft+1
	end
	local cg=g:Filter(Card.IsFacedown,nil)
	if cg:GetCount()>0 then
		Duel.ConfirmCards(1-tp,cg)
	end
	Duel.SendtoDeck(g,nil,2,REASON_COST)
end
function c78512663.atkval(e,c)
	return Duel.GetFieldGroupCount(0,LOCATION_ONFIELD,LOCATION_ONFIELD)*400
end
function c78512663.retcon1(e,tp,eg,ep,ev,re,r,rp,chk)
	return not e:GetHandler():IsHasEffect(42015635)
end
function c78512663.retcon2(e,tp,eg,ep,ev,re,r,rp,chk)
	return e:GetHandler():IsHasEffect(42015635)
end
function c78512663.rettg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToExtra() end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,e:GetHandler(),1,0,0)
end
function c78512663.retop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	Duel.SendtoDeck(c,nil,2,REASON_EFFECT)
	if c:IsLocation(LOCATION_EXTRA) then
		Duel.RaiseSingleEvent(c,78512663,e,0,0,0,0)
	end
end
function c78512663.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(Card.IsAbleToHand,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c78512663.thop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsAbleToHand,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	Duel.SendtoHand(g,nil,REASON_EFFECT)
end
