package com.accenture.rac.wizards.importer.bom;

import java.math.BigDecimal;

import com.accenture.rac.wizards.importer.AbstractRow;

public class BomLine extends AbstractRow{
	
	private static final long serialVersionUID = -6313954804407671679L;
	protected String itemId;
	protected String revisionId;
	protected String refDes;
	protected String findNo;
	protected String quantity;
	protected String uom;
	protected String latestStatus;
	protected Boolean has3D;
	
	protected BigDecimal r00 = BigDecimal.ONE; 
	protected BigDecimal r10 = BigDecimal.ZERO; 
	protected BigDecimal r20 = BigDecimal.ZERO; 
	protected BigDecimal p0  = BigDecimal.ZERO; 
	protected BigDecimal r01 = BigDecimal.ZERO;
	protected BigDecimal r11 = BigDecimal.ONE;
	protected BigDecimal r21 = BigDecimal.ZERO;
	protected BigDecimal p1  = BigDecimal.ZERO;
	protected BigDecimal r02 = BigDecimal.ZERO;
	protected BigDecimal r12 = BigDecimal.ZERO;
	protected BigDecimal r22 = BigDecimal.ONE;
	protected BigDecimal p2  = BigDecimal.ZERO;
	protected BigDecimal t0  = BigDecimal.ZERO; //x
	protected BigDecimal t1  = BigDecimal.ZERO; //y
	protected BigDecimal t2  = BigDecimal.ZERO; //z
	protected BigDecimal s   = BigDecimal.ONE;
	
	private String transformation;
	
	@Override
	public String getId() {
		return ((itemId == null) ? "" : itemId ) + ((refDes == null) ? "" : refDes);
	}

	@Override
	public boolean equalId(Object obj) {
		if(obj == null || !(obj instanceof BomLine))
			return false;
		
		BomLine bomLine = ((BomLine) obj);
		
		if(bomLine.getId().isEmpty())
			return false;
		
		String thisId = this.getId();
		String thatId = bomLine.getId();
		
		if(thisId.equals(thatId))
			return true;
		
		return false;
		
	}
	
	public boolean equalObject(Object obj) {
		
		if(this.equalId(obj)){
			BomLine b = (BomLine) obj;

			if(getItemId() != null && !getItemId().equalsIgnoreCase(b.getItemId()))
				return false;
			
			if(getFindNo() != null && !getFindNo().equalsIgnoreCase(b.getFindNo()))
				return false;
			
			if(getRevisionId() != null && !getRevisionId().equalsIgnoreCase(b.getRevisionId()))
				return false;
			
			if(getTransformation() != null && !getTransformation().equals(b.getTransformation()))
				return false;
			
			
			// null checks
			
			if(getItemId() == null && b.getItemId() != null)
				return false;
			
			if(getFindNo() == null && b.getFindNo() != null)
				return false;
			
			if(getRevisionId() == null && b.getRevisionId() != null)
				return false;
			
			if(getTransformation() == null && b.getTransformation() != null)
				return false;
			
			return true;
		}
		
		return false;
	}
	
	public boolean equalQuantity(Object obj){
		
		if(obj instanceof BomLine){
			BomLine b = (BomLine) obj;
			
			//if(getQuantityAsInteger() == b.getQuantityAsInteger())
			//	return true;
			Double thisQuantity = 0.0;
			Double cmpQuantity = 0.0;
			
			if (quantity == null || quantity.isEmpty() )
			{	
				if (uom == null || uom.compareToIgnoreCase("each") == 0)
				{
					quantity = "1";
				}
				else
				{
					quantity = "1.0";
				}
				thisQuantity = 1.0;
			}
			else
			{
				thisQuantity = Double.valueOf(quantity);
			}
			
			if (b.getQuantity() == null || b.getQuantity().isEmpty() )
			{	
				if (b.uom == null || b.uom.compareToIgnoreCase("each") == 0)
				{
					b.setQuantity("1");
				}
				else
				{
					b.setQuantity("1.0");
				}
				
				cmpQuantity = 1.0;
			}
			else
			{
				cmpQuantity = Double.valueOf(b.getQuantity());
			}

			double epsilon = 0.0000000000000000000000000000000001;
			
			if (Math.abs(thisQuantity - 0.0) < epsilon)
			{
				thisQuantity += 1.0;
			}
			
			if (Math.abs(cmpQuantity - 0.0) < epsilon)
			{
				cmpQuantity += 1.0;
			}
			
			if ( uom.compareToIgnoreCase(b.getUom()) !=0 )
			{
				return false;
			}
			
			if (Math.abs(thisQuantity - cmpQuantity) < epsilon)
			{
				return true;
			}
			
		}
		
		return false;
	}
	

	@Override
	public String toString() {
		return "BomLine [itemId=" + itemId
				+ ", revisionId=" + revisionId + ", refDes=" + refDes
				+ ", findNo=" + findNo + ", uom=" + uom 
				+ ", quantity=" + quantity 
				+ ", matrix=" + getTransformation() + "]";
	}
	
	public String getItemId() {
		return itemId;
	}
	public void setItemId(String itemId) {
		this.itemId = itemId;
	}
	public String getRevisionId() {
		return revisionId;
	}
	public void setRevisionId(String revisionId) {
		this.revisionId = revisionId;
	}
	public String getRefDes() {
		return refDes;
	}
	public void setRefDes(String refDes) {
		this.refDes = refDes;
	}
	public String getFindNo() {
		return findNo;
	}
	public void setFindNo(String findNo) {
		this.findNo = findNo;
	}
	public String getQuantity() {
		return quantity;
	}
	
	public int getQuantityAsInteger() {
		if(getQuantity() != null && !getQuantity().isEmpty()){
			return Integer.parseInt(getQuantity()) ;
		}
		return 1;
	}
	
	public void setQuantityAsInteger(int quantity) {
		setQuantity(String.valueOf(quantity));
	}
	
	public void setQuantity(String quantity) {
		this.quantity = quantity;
	}
	
	public void setUom(String uom) {
		this.uom = uom;
	}
	
	public String getUom() {
		return uom;
	}

	public void setQuantitySubtract() {
		int q =getQuantityAsInteger();
		if(--q > 0){
			setQuantityAsInteger(q);
		}else{
			setQuantityAsInteger(1);
		}
	}
	
	public void setQuantityAdd() {
		int q = getQuantityAsInteger() + 1;
		setQuantityAsInteger(q);
	}
	
	public void setQuantityAdd(int quantity) {
		int q = getQuantityAsInteger() + quantity;
		setQuantityAsInteger(q);
	}

	/**
	 * @return the transformation
	 */
	public String getTransformation() {
		this.transformation = String.format("%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
				getR00(),
				getR10(),
				getR20(),
				getP0(),
				getR01(),
				getR11(),
				getR21(),
				getP1(),
				getR02(),
				getR12(),
				getR22(),
				getP2(),
				getT0(),
				getT1(),
				getT2(),
				getS()
				);
		return transformation;
		
	}

	/**
	 * @param transformation the transformation to set
	 */
	public void setTransformation(String transformation) {
		if(transformation == null){
			setDefaultTransformation();
			return;
		}
		
		String[] transStrArray = transformation.split(" ");
		BigDecimal[] transArray = new BigDecimal[16];
		
		for (int i = 0; i < transStrArray.length; i++) {
			try{
				transArray[i] = new BigDecimal(transStrArray[i]); //BigDecimal.valueOf(Double.valueOf(transStrArray[i]));
			}catch (Exception e) {
				transArray[i] = BigDecimal.ZERO;
			}
		}
		
		setR00(transArray[0]);
		setR10(transArray[1]);
		setR20(transArray[2]);
		setP0(transArray[3]);
		setR01(transArray[4]);
		setR11(transArray[5]);
		setR21(transArray[6]);
		setP1(transArray[7]);
		setR02(transArray[8]);
		setR12(transArray[9]);
		setR22(transArray[10]);
		setP2(transArray[11]);
		setT0(transArray[12]);
		setT1(transArray[13]);
		setT2(transArray[14]);
		setS(transArray[15]);
		
	}
	
	public void setDefaultTransformation() {
		r00 = BigDecimal.ONE; 
		r10 = BigDecimal.ZERO; 
		r20 = BigDecimal.ZERO; 
		p0  = BigDecimal.ZERO; 
		r01 = BigDecimal.ZERO;
		r11 = BigDecimal.ONE;
		r21 = BigDecimal.ZERO;
		p1  = BigDecimal.ZERO;
		r02 = BigDecimal.ZERO;
		r12 = BigDecimal.ZERO;
		r22 = BigDecimal.ONE;
		p2  = BigDecimal.ZERO;
		t0  = BigDecimal.ZERO; //x
		t1  = BigDecimal.ZERO; //y
		t2  = BigDecimal.ZERO; //z
		s   = BigDecimal.ONE;
		
	}

	/**
	 * @return the r00
	 */
	public BigDecimal getR00() {
		return r00;
	}

	/**
	 * @param r00 the r00 to set
	 */
	public void setR00(BigDecimal r00) {
		this.r00 = r00;
	}

	/**
	 * @return the r10
	 */
	public BigDecimal getR10() {
		return r10;
	}

	/**
	 * @param r10 the r10 to set
	 */
	public void setR10(BigDecimal r10) {
		this.r10 = r10;
	}

	/**
	 * @return the r20
	 */
	public BigDecimal getR20() {
		return r20;
	}

	/**
	 * @param r20 the r20 to set
	 */
	public void setR20(BigDecimal r20) {
		this.r20 = r20;
	}

	/**
	 * @return the p0
	 */
	public BigDecimal getP0() {
		return p0;
	}

	/**
	 * @param p0 the p0 to set
	 */
	public void setP0(BigDecimal p0) {
		this.p0 = p0;
	}

	/**
	 * @return the r01
	 */
	public BigDecimal getR01() {
		return r01;
	}

	/**
	 * @param r01 the r01 to set
	 */
	public void setR01(BigDecimal r01) {
		this.r01 = r01;
	}

	/**
	 * @return the r11
	 */
	public BigDecimal getR11() {
		return r11;
	}

	/**
	 * @param r11 the r11 to set
	 */
	public void setR11(BigDecimal r11) {
		this.r11 = r11;
	}

	/**
	 * @return the r21
	 */
	public BigDecimal getR21() {
		return r21;
	}

	/**
	 * @param r21 the r21 to set
	 */
	public void setR21(BigDecimal r21) {
		this.r21 = r21;
	}

	/**
	 * @return the p1
	 */
	public BigDecimal getP1() {
		return p1;
	}

	/**
	 * @param p1 the p1 to set
	 */
	public void setP1(BigDecimal p1) {
		this.p1 = p1;
	}

	/**
	 * @return the r02
	 */
	public BigDecimal getR02() {
		return r02;
	}

	/**
	 * @param r02 the r02 to set
	 */
	public void setR02(BigDecimal r02) {
		this.r02 = r02;
	}

	/**
	 * @return the r12
	 */
	public BigDecimal getR12() {
		return r12;
	}

	/**
	 * @param r12 the r12 to set
	 */
	public void setR12(BigDecimal r12) {
		this.r12 = r12;
	}

	/**
	 * @return the r22
	 */
	public BigDecimal getR22() {
		return r22;
	}

	/**
	 * @param r22 the r22 to set
	 */
	public void setR22(BigDecimal r22) {
		this.r22 = r22;
	}

	/**
	 * @return the p2
	 */
	public BigDecimal getP2() {
		return p2;
	}

	/**
	 * @param p2 the p2 to set
	 */
	public void setP2(BigDecimal p2) {
		this.p2 = p2;
	}

	/**
	 * @return the t0
	 */
	public BigDecimal getT0() {
		return t0;
	}

	/**
	 * @param t0 the t0 to set
	 */
	public void setT0(BigDecimal t0) {
		this.t0 = t0;
	}

	/**
	 * @return the t1
	 */
	public BigDecimal getT1() {
		return t1;
	}

	/**
	 * @param t1 the t1 to set
	 */
	public void setT1(BigDecimal t1) {
		this.t1 = t1;
	}

	/**
	 * @return the t2
	 */
	public BigDecimal getT2() {
		return t2;
	}

	/**
	 * @param t2 the t2 to set
	 */
	public void setT2(BigDecimal t2) {
		this.t2 = t2;
	}

	/**
	 * @return the s
	 */
	public BigDecimal getS() {
		return s;
	}

	/**
	 * @param s the s to set
	 */
	public void setS(BigDecimal s) {
		this.s = s;
	}

	/**
	 * @return the latestStatus
	 */
	public String getLatestStatus() {
		return latestStatus;
	}

	/**
	 * @param latestStatus the latestStatus to set
	 */
	public void setLatestStatus(String latestStatus) {
		this.latestStatus = latestStatus;
	}

	/**
	 * @return the has3D
	 */
	public Boolean getHas3D() {
		return has3D;
	}

	/**
	 * @param has3d the has3D to set
	 */
	public void setHas3D(Boolean has3d) {
		has3D = has3d;
	}

}
