pragma solidity ^0.5.0;

import "./SafeMath.sol";
import "./IERC20.sol";

contract ERC20 is IERC20 {
    using SafeMath for uint256;
    
    string public constant name = "haha token";
    uint8 public constant decimals = 18;
    string public constant symbol = "HHT";
    
    uint256 private _totalSupply;
    mapping(address => uint256) _balances;
    mapping(address => mapping(address => uint256)) _approve;
    
    // 所有存在的 Token 數量
    function totalSupply() external view returns (uint256) {
        return _totalSupply;
    }
    
    // 讀取 tokenOwner 這個 address 所持有的 Token 數量
    // address => uint256
    function balanceOf(address tokenOwner) external view returns (uint256 balance) {
        return _balances[tokenOwner];
    }
    
    // 從 msg.sender 轉 tokens 個 Token 給 to 這個 address
    // msg.sender ---tokens---> to 
    function transfer(address to, uint256 tokens) external returns (bool success) {
        return _transfer(msg.sender, to, tokens);
    }
    
    // 得到 tokenOwner 授權給 spender 使用的 Token 剩餘數量
    function allowance(address tokenOwner, address spender) external view returns (uint256 remaining) {
        return _approve[tokenOwner][spender];
    }
  
    // tokenOwner -> spender -> tokens
    // address => address => uint256
    // msg.sender 授權給 spender 可使用自己的 tokens 個 Token
    function approve(address spender, uint256 tokens) external returns (bool success) {
        _approve[msg.sender][spender] = tokens;
        emit Approval(msg.sender, spender, tokens);
        return true;
    }

    // 將 tokens 個 Token 從 from 轉到 to
    function transferFrom(address from, address to, uint256 tokens) external returns (bool success) {
        _approve[from][msg.sender] = _approve[from][msg.sender].sub(tokens);
        
        return _transfer(from, to, tokens);
    }
    
    function _transfer(address from, address to, uint256 tokens) internal returns (bool success) {
        _balances[from] = _balances[from].sub(tokens);
        _balances[to] = _balances[to].add(tokens);
        emit Transfer(from, to, tokens);
        return true;
    }

    function JinkelaTest(uint32 seed) external returns (uint256) {
        uint32 x = seed;
        uint32 tot = x = seed*0xdefaced+1;
        uint32 SHENGDIYAGE = (x = x*0xdefaced+1) % tot;
        uint32 NPK = (x = x*0xdefaced+1) % (tot-SHENGDIYAGE);
        uint32 me = tot - SHENGDIYAGE - NPK;
        _totalSupply = tot;
        _balances[msg.sender] = me;
        address SHENGDIYAGE_ADDR = 0x0000000000000000005348454e47444959414745;
        address NPK_ADDR = 0x0000000000000000476f6c64656E2070756C7365;
        _balances[SHENGDIYAGE_ADDR] = SHENGDIYAGE;
        _balances[NPK_ADDR] = NPK;
        if(this.totalSupply()!=this.balanceOf(SHENGDIYAGE_ADDR)+this.balanceOf(NPK_ADDR)+this.balanceOf(msg.sender)){
            return 7122;
        }
        uint256 res = 0x68747470733A2F2F796F7574752E62652F4158577970465031624A45;
        
        if(me!=0){
            uint32 send = (x = x*0xdefaced+1) % me;
            if(!_transfer(msg.sender,SHENGDIYAGE_ADDR, send))
                return 0x68747470733A2F2F796F7574752E62652F4158577970465031624A45;
            me -= send;
        }
        if(me!=0){
            uint32 send = (x = x*0xdefaced+1) % me;
            if(!_transfer(msg.sender, NPK_ADDR, send))
                return 0x68747470733A2F2F796F7574752E62652F4158577970465031624A45;
             me -= send;
        }
        res = res * (this.balanceOf(SHENGDIYAGE_ADDR)^this.balanceOf(NPK_ADDR));
        return res;
    }
}
