package com.accenture.sbt.rac.objects;

import java.math.BigDecimal;

import com.accenture.rac.wizards.importer.bom.BomLine;


public class SbtBomLine extends BomLine{
	
	private static final long serialVersionUID = 8444115278327807842L;
	private Integer rotation = null;
	private String placement = null;

	public SbtBomLine() {
		this.setFindNo("0");
	}
	
	@Override
	public String getId() {
		return ((getFindNo() == null) ? "" : getFindNo() ) + ((getRefDes() == null) ? "" : getRefDes());
	}

	public Integer getRotation() {
		return rotation;
	}

	public void setRotation(int rotation) {
		this.rotation = rotation;
		calculateRotationMatrix();
	}

	public String getPlacement() {
		return placement;
	}

	public void setPlacement(String placement) {
		this.placement = placement;
		calculateRotationMatrix();
	}
	
	public void calculateRotationMatrix(){
		if(getPlacement() == null || getRotation() == null){
			return;
		}
		
		if(placement.equalsIgnoreCase("top") && rotation == 0){
			setRotationMatrix(1,0,0,0,1,0,0,0,1);
			return;
		}
		
		if(placement.equalsIgnoreCase("top") && rotation == 90){
			setRotationMatrix(0,1,0,-1,0,0,0,0,1);
			return;
		}
		
		if(placement.equalsIgnoreCase("top") && rotation == 180){
			setRotationMatrix(-1,0,0,0,-1,0,0,0,1);
			return;
		}
		
		if(placement.equalsIgnoreCase("top") && rotation == 270){
			setRotationMatrix(0,-1,0,1,0,0,0,0,1);
			return;
		}

		if(placement.equalsIgnoreCase("bottom") && rotation == 0){
			//TODO: set rotation matrix 
			return;
		}

		if(placement.equalsIgnoreCase("bottom") && rotation == 90){
			//TODO: set rotation matrix
			return;
		}
		
		if(placement.equalsIgnoreCase("bottom") && rotation == 180){
			//TODO: set rotation matrix
			return;
		}
		
		if(placement.equalsIgnoreCase("bottom") && rotation == 270){
			setRotationMatrix(0,-1,0,-1,0,0,0,0,-1);
			return;
		}
	}
	
	public void setRotationMatrix(int R00, int R10, int R20, int R01, int R11, int R21, int R02, int R12, int R22){
		setR00(Bd(R00));
		setR10(Bd(R10));
		setR20(Bd(R20));
		setR01(Bd(R01));
		setR11(Bd(R11));
		setR21(Bd(R21));
		setR02(Bd(R02));
		setR12(Bd(R12));
		setR22(Bd(R22));
	}
	
	public BigDecimal Bd(int i){
		return new BigDecimal(i);
	}
	
	

}
